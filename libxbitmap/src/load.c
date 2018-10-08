#include <ximg/xbitmap.h>
#include <ximg/xras.h>
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

static inline int xbitmap_load_headers(FILE * f, struct xbitmap_file * file, struct xbitmap_header * header){
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Check if the file is large enough to contain the minimum headers
    if(size < (2 + sizeof(struct xbitmap_file) + sizeof(struct xbitmap_header))) return 0;

    // File identifier should contain BM (aka 0x4D42 little endian)
    uint16_t identifier;
    if(!fread(&identifier, 2, 1, f) || identifier != 0x4D42) return 0;

    // Load file descriptor
    if(!fread(file, sizeof(struct xbitmap_file), 1, f)) return 0;

    // When the actual file size is smaller then what the header tells us, it must be corrupt
    if(size < file->size) return 0;

    // Load the header
    if(!fread(header, sizeof(struct xbitmap_header), 1, f)) return 0;

    // If the header is not of an size we support return false
    if(header->size != sizeof(struct xbitmap_header)) return 0;

    // It looks like we can read this file
    return 1;
}

static inline int xbitmap_load_raster_24bit(FILE * f, struct xbitmap_file * file, struct xbitmap_header * header, struct ximg * image){
    ximgid_t raster_id = xras_create(image, header->width, header->height, ximg_make("RGB8"), 3);

    struct xras * raster = xras_get_by_id(image, raster_id);

    struct xchan * r = xras_channel(image, raster, 0);
    struct xchan * g = xras_channel(image, raster, 1);
    struct xchan * b = xras_channel(image, raster, 2);

    fseek(f, file->offset, SEEK_SET);
    unsigned int x, y = header->height - 1;
    struct xbitmap_rgba pixel;

    struct xbitmap_reader reader;
    xbitmap_reader_init(&reader, file->offset, header->bits, header->width);
    for(;;){
        xbitmap_reader_nextline(&reader, f);
        x = 0;
        while(x < header->width){
            fread(&pixel, 3, 1, f);

            xchan_set8(r, x, y, pixel.r);
            xchan_set8(g, x, y, pixel.g);
            xchan_set8(b, x, y, pixel.b);
            x++;
        }
        if(y == 0) break;
        y--;
    }
    return 1;
}

static inline int xbitmap_load_raster_32bit(FILE * f, struct xbitmap_file * file, struct xbitmap_header * header, struct ximg * image){
    ximgid_t raster_id = xras_create(image, header->width, header->height, ximg_make("RGBA"), 4);

    struct xras * raster = xras_get_by_id(image, raster_id);

    struct xchan * r = xras_channel(image, raster, 0);
    struct xchan * g = xras_channel(image, raster, 1);
    struct xchan * b = xras_channel(image, raster, 2);
    struct xchan * a = xras_channel(image, raster, 3);

    fseek(f, file->offset, SEEK_SET);
    unsigned int x, y = header->height - 1;
    struct xbitmap_rgba pixel;

    for(;;){
        x = 0;
        while(x < header->width){
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
    return 1;
}

static inline int xbitmap_load_raster(FILE * f, struct xbitmap_file * file, struct xbitmap_header * header, struct ximg * image){
    // When compression is set 24bit RLE or BITFIELD are on 
    if(header->compression) return 0;

    if(header->bits == 24) return xbitmap_load_raster_24bit(f, file, header, image);
    if(header->bits == 32) return xbitmap_load_raster_32bit(f, file, header, image);
    return 0;
}

static inline int xbitmap_load_mapped(FILE * f, struct xbitmap_file * file, struct xbitmap_header * header, struct ximg * image){
    ximgid_t palette_id = xpal_create(image, XPAL_RGB8, header->colors);
    if(!palette_id) return 0;

    struct xpal * palette = xpal_get_by_id(image, palette_id);
    if(!palette) return 0;

    struct xpixel color;
    for(int i = 0; i < header->colors; i++){
        if(!fread(&color, 4, 1, f)) return 0; 
        xpal_set_rgb(palette, i, &color);
    }

    fseek(f, file->offset, SEEK_SET);
    unsigned int x, y = header->height - 1;
    struct xbitmap_rgba pixel;


    ximgid_t mapped_id = xmap_create_with_palette(image, header->width, header->height, palette_id);
    if(!mapped_id) return 0;

    struct xchan * channel = xmap_channel(image, xmap_get_by_id(image, mapped_id));

    struct xbitmap_reader reader;
    xbitmap_reader_init(&reader, file->offset, header->bits, header->width);
    for(;;){
        xbitmap_reader_nextline(&reader, f);
        x = 0;
        while(x < header->width){
            uint32_t index = xbitmap_reader_next(&reader, f);
            xchan_set8(channel, x, y, index);
            x++;
        }
        if(y == 0) break;
        y--;
    }
}

struct ximg * xbitmap_load(const char * filename){
    FILE * f = fopen(filename, "rb");
    if(!f) return 0;

    // Load the bitmap headers
    struct xbitmap_file file;
    struct xbitmap_header header;
    if(!xbitmap_load_headers(f, &file, &header)){
        fclose(f);
        return 0;
    }

    struct ximg * image = ximg_create();

    if(header.bits > 8){
        if(!xbitmap_load_raster(f, &file, &header, image)){
            ximg_free(image);
            return 0;
        }
    }else{
        if(!xbitmap_load_mapped(f, &file, &header, image)){
            ximg_free(image);
            return 0;
        }
    }

    fclose(f);

    return image;
}