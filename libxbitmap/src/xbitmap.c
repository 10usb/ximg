#include <ximg/xreader.h>
#include <ximg/xmap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct xbitmap_file {
  uint32_t size;            // (4 bytes),
  uint32_t reserved;        // (4 bytes),
  uint32_t offset;          // (4 bytes), offset to the data
} __attribute__((__packed__));

struct xbitmap_header {
  uint32_t size;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bits;      // 1,4,8,24,32
  uint32_t compression;
  uint32_t data_size;
  uint32_t xppm;
  uint32_t yppm;
  uint32_t colors;
  uint32_t important;
} __attribute__((__packed__));

struct xbitmap_rgba {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t a;
} __attribute__((__packed__));

struct xbitmap_reader {
    uint32_t offset;
    int bits;
    uint32_t scanline;
    int position;
    uint32_t buffer;
};

void xbitmap_reader_init(struct xbitmap_reader * reader, uint32_t offset, int bits, uint32_t width){
    reader->offset = offset;
    reader->bits = bits;
    reader->scanline = bits * width + 31 - (bits * width - 1) % 32;
    reader->position = bits;
}

void xbitmap_reader_nextline(struct xbitmap_reader * reader, FILE * f){
    long position = ftell(f);
    long remain = (position - reader->offset) % reader->scanline;
    if(remain > 0) fseek(f, reader->scanline - remain, SEEK_CUR);
    reader->position = reader->bits;
}

int xbitmap_read_32bit_le(FILE * f, uint32_t * value){
    // for now assume we are compiling on a great system
    return fread(value, sizeof(uint32_t), 1, f);
}

uint32_t xbitmap_reader_next(struct xbitmap_reader * reader, FILE * f){
    uint32_t value = reader->buffer >> reader->position;
    reader->position+= reader->bits;

    if(reader->position >= 32){
        reader->position-= 32;
        xbitmap_read_32bit_le(f, &reader->buffer);
        
        if(reader->position > 0) value |= reader->buffer << (32 - reader->position);
    }
    return value & ((1 << reader->bits)  - 1);
}

struct ximg * xbitmap_load(const char * filename){
    FILE * f = fopen(filename, "rb");
    if(!f) return 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(size < (2 + sizeof(struct xbitmap_file) + sizeof(struct xbitmap_header))){
        fclose(f);
        return 0;
    }

    uint16_t id;
    fread(&id, 2, 1, f);

    if(id != 0x4D42){
        fclose(f);
        return 0;
    }

    struct xbitmap_file file;
    fread(&file, sizeof(struct xbitmap_file), 1, f);

    if(size < file.size){
        fclose(f);
        return 0;
    }

    struct xbitmap_header header;
    fread(&header, sizeof(struct xbitmap_header), 1, f);

    if(header.size != sizeof(struct xbitmap_header)){
        fclose(f);
        return 0;
    }

    if(header.compression){
        fclose(f);
        return 0;
    }

    struct ximg * image = 0;

    switch(header.bits){
        case 32:{
            image = ximg_create();
            unsigned int raster_index = xras_create(image, header.width, header.height, ximg_make("RGBA"), 4);

            struct xras * raster = xras_get_by_id(image, raster_index);

            struct xchan * r = xras_channel(image, raster, 0);
            struct xchan * g = xras_channel(image, raster, 1);
            struct xchan * b = xras_channel(image, raster, 2);
            struct xchan * a = xras_channel(image, raster, 3);

            fseek(f, file.offset, SEEK_SET);
            unsigned int x, y = header.height - 1;
            struct xbitmap_rgba pixel;

            while(y > 0){
                x = 0;
                while(x < header.width){
                    fread(&pixel, 4, 1, f);

                    xchan_set8(r, x, y, pixel.r);
                    xchan_set8(g, x, y, pixel.g);
                    xchan_set8(b, x, y, pixel.b);
                    xchan_set8(a, x, y, pixel.a);
                    x++;
                }
                if(y == 0) break;
                y--;
            }
        }break;
        case 24:{
            image = ximg_create();
            unsigned short raster_index = xras_create(image, header.width, header.height, ximg_make("RGB8"), 3);

            struct xras * raster = xras_get_by_id(image, raster_index);

            struct xchan * r = xras_channel(image, raster, 0);
            struct xchan * g = xras_channel(image, raster, 1);
            struct xchan * b = xras_channel(image, raster, 2);

            fseek(f, file.offset, SEEK_SET);
            unsigned int x, y = header.height - 1;
            struct xbitmap_rgba pixel;

            for(;;){
                x = 0;
                while(x < header.width){
                    fread(&pixel, 3, 1, f);

                    xchan_set8(r, x, y, pixel.r);
                    xchan_set8(g, x, y, pixel.g);
                    xchan_set8(b, x, y, pixel.b);
                    x++;
                }
                if(y == 0) break;
                y--;
            }
        }break;
        default:
            if(!header.compression){
                image = ximg_create();

                ximgid_t palette_id = xpal_create(image, XPAL_RGB8, header.colors);
                if(!palette_id) {
                    ximg_free(image);
                    fclose(f);
                    return 0;
                }

                struct xpal * palette = xpal_get_by_id(image, palette_id);
                if(!palette){
                    ximg_free(image);
                    fclose(f);
                    return 0;
                }

                struct xpixel color;
                for(int i = 0; i < header.colors; i++){
                    if(!fread(&color, 4, 1, f)){
                        ximg_free(image);
                        fclose(f);
                        return 0;
                    }
                    xpal_set_rgb(palette, i, &color);
                }

                fseek(f, file.offset, SEEK_SET);
                unsigned int x, y = header.height - 1;
                struct xbitmap_rgba pixel;

                struct xbitmap_reader reader;
                xbitmap_reader_init(&reader, file.offset, header.bits, header.width);

                ximgid_t mapped_id = xmap_create_with_palette(image, header.width, header.height, palette_id);
                if(!mapped_id){
                    ximg_free(image);
                    fclose(f);
                    return 0;
                }

                struct xchan * channel = xmap_channel(image, xmap_get_by_id(image, mapped_id));

                for(;;){
                    xbitmap_reader_nextline(&reader, f);
                    x = 0;
                    while(x < header.width){
                        uint32_t index = xbitmap_reader_next(&reader, f);
						xchan_set8(channel, x, y, index);
                        x++;
                    }
                    if(y == 0) break;
                    y--;
                }

            }else{
                printf("file size  : %d\n", file.size);
                printf("offset     : %d\n", file.offset);

                printf("size       : %d\n", header.size);
                printf("width      : %d\n", header.width);
                printf("height     : %d\n", header.height);
                printf("planes     : %d\n", header.planes);
                printf("bits       : %d\n", header.bits);
                printf("compression: %d\n", header.compression);
                printf("data_size  : %d\n", header.data_size);
                printf("xppm       : %d\n", header.xppm);
                printf("yppm       : %d\n", header.yppm);
                printf("colors     : %d\n", header.colors);
                printf("important  : %d\n", header.important);
            }
        break;
    }

    fclose(f);

    return image;
}

int xbitmap_save(struct ximg * image, uint16_t index, const char * filename, uint8_t bits){
    struct xreader reader;
    if(!xreader_init(&reader, image, index)) return 0;

    unsigned short scanline = 0, padding = 0;
    switch(bits){
        case 32:{
            scanline = reader.width * 4;
        }break;
        case 24:{
            padding  = 3 - (reader.width * 3 - 1) % 4;
            scanline = reader.width * 3 + padding;
        }break;
        default:{
            xreader_clear(&reader);
        }return 0;
    }



    struct xbitmap_file file;
    memset(&file, 0, sizeof(struct xbitmap_file));
    file.offset        = 2 + sizeof(struct xbitmap_file) + sizeof(struct xbitmap_header);

    struct xbitmap_header header;
    memset(&header, 0, sizeof(struct xbitmap_header));
    header.size         = sizeof(struct xbitmap_header);
    header.width        = reader.width;
    header.height       = reader.height;
    header.planes       = 1;
    header.bits         = bits;
    header.data_size    = scanline * reader.height;
    header.xppm         = 2834; // 72dpi
    header.yppm         = 2834; // 72dpi

    file.size           = file.offset + header.data_size;

    FILE * f = fopen(filename, "wb");
    if(!f){
        xreader_clear(&reader);
        return 0;
    }

    unsigned short id = 0x4D42;
    fwrite(&id, 2, 1, f);
    fwrite(&file, sizeof(struct xbitmap_file), 1, f);
    fwrite(&header, sizeof(struct xbitmap_header), 1, f);


    unsigned int x, y = header.height - 1;
    struct xpixel xpixel;
    struct xbitmap_rgba bpixel;

    if(bits==32){
        for(;;){
            x = 0;
            while(x < header.width){
                xreader_rgba(&reader, x, y, &xpixel);
                bpixel.r = xpixel.r;
                bpixel.g = xpixel.g;
                bpixel.b = xpixel.b;
                bpixel.a = xpixel.a;
                fwrite(&bpixel, 4, 1, f);
                x++;
            }
            if(y == 0) break;
            y--;
        }
    }else if(bits==24){
        for(;;){
            x = 0;
            while(x < header.width){
                xreader_rgb(&reader, x, y, &xpixel);
                bpixel.r = xpixel.r;
                bpixel.g = xpixel.g;
                bpixel.b = xpixel.b;
                fwrite(&bpixel, 3, 1, f);
                x++;
            }
            fwrite("\0\0\0", 1, padding, f);
            if(y == 0) break;
            y--;
        }
    }


    fclose(f);

    return 1;
}
