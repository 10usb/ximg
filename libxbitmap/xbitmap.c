#include <ximg/xreader.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct xbitmap_file {
  unsigned int size;            // (4 bytes),
  unsigned int reserved;        // (4 bytes),
  unsigned int offset;          // (4 bytes), offset to the data
} __attribute__((__packed__));

struct xbitmap_header {
  unsigned int size;
  unsigned int width;
  unsigned int height;
  unsigned short planes;
  unsigned short bits;      // 1,4,8,24,32
  unsigned int compression;
  unsigned int data_size;
  unsigned int xppm;
  unsigned int yppm;
  unsigned int colors;
  unsigned int important;
} __attribute__((__packed__));

struct xbitmap_rgba {
  unsigned char b;
  unsigned char g;
  unsigned char r;
  unsigned char a;
} __attribute__((__packed__));

struct ximg * xbitmap_load(const char * filename){
    FILE * f = fopen(filename, "rb");
    if(!f) return 0;

    fseek(f, 0, SEEK_END);
    unsigned int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(size < (2 + sizeof(struct xbitmap_file) + sizeof(struct xbitmap_header))){
        fclose(f);
        return 0;
    }

    unsigned short id;
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

    struct ximg * image = ximg_create();

    switch(header.bits){
        case 32:{
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
    }

    fclose(f);

    return image;
}

int xbitmap_save(struct ximg * image, unsigned short index, const char * filename, unsigned char bits){
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
