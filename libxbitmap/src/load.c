#include <ximg/xbitmap.h>
#include <ximg/xreader.h>
#include <ximg/xmap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"

void xbitmap_reader_init(struct xbitmap_reader * reader, uint32_t offset, int bits, uint32_t width){
    reader->offset = offset;
    reader->bits = bits;
    reader->scanline = (bits * width + 31 - (bits * width - 1) % 32) / 8;
    reader->position = 0;
    reader->buffer = 0;
    reader->mask = (1 << bits)  - 1;
}

void xbitmap_reader_nextline(struct xbitmap_reader * reader, FILE * f){
    long position = ftell(f);
    long remain = (position - reader->offset) % reader->scanline;
    if(remain > 0) fseek(f, reader->scanline - remain, SEEK_CUR);
    reader->position = 0;
}


uint32_t xbitmap_reader_next(struct xbitmap_reader * reader, FILE * f){
    if(reader->bits == 8){
        uint8_t value;
        fread(&value, sizeof(uint8_t), 1, f);
        return value;
    }else{
        uint8_t value;

        reader->position-= reader->bits;

        if(reader->position < 0){
            value = reader->buffer << (0 - reader->position);

            fread(&reader->buffer, sizeof(uint8_t), 1, f);
            
            uint8_t mask = (1 << (0 - reader->position))  - 1;
            value|= (reader->buffer >> (reader->position + 8)) & mask;

            reader->position+= 8;
        }else{
            value = reader->buffer >> reader->position;
        }
        
        return value & reader->mask;
    }
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

            struct xbitmap_reader reader;
            xbitmap_reader_init(&reader, file.offset, header.bits, header.width);
            for(;;){
                xbitmap_reader_nextline(&reader, f);
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


                ximgid_t mapped_id = xmap_create_with_palette(image, header.width, header.height, palette_id);
                if(!mapped_id){
                    ximg_free(image);
                    fclose(f);
                    return 0;
                }

                struct xchan * channel = xmap_channel(image, xmap_get_by_id(image, mapped_id));

                struct xbitmap_reader reader;
                xbitmap_reader_init(&reader, file.offset, header.bits, header.width);
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