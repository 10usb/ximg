#include <ximg/xbitmap.h>
#include <ximg/xreader.h>
#include <ximg/xmap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.h"

int xbitmap_raster_save(struct ximg * image, uint16_t index, const char * filename, uint8_t bits){
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

int xbitmap_mapped_save(struct ximg * image, uint16_t index, const char * filename, uint8_t bits){
    struct xmap * mapped = xmap_get_by_index(image, index);
    if(!mapped) return 0;

    struct xpal * palette = xmap_palette(image, mapped);
    if(!palette) return 0;

    struct xchan * channel = xmap_channel(image, mapped);
    if(!channel) return 0;
    

    unsigned short scanline = 0, padding = 0;
    scanline = (bits * channel->width + 31 - (bits * channel->width - 1) % 32) / 8;
    padding = scanline - ((bits * channel->width + 7 - (bits * channel->width - 1) % 8) / 8);

    struct xbitmap_file file;
    memset(&file, 0, sizeof(struct xbitmap_file));
    file.offset        = 2 + sizeof(struct xbitmap_file) + sizeof(struct xbitmap_header);

    struct xbitmap_header header;
    memset(&header, 0, sizeof(struct xbitmap_header));
    header.size         = sizeof(struct xbitmap_header);
    header.width        = channel->width;
    header.height       = channel->height;
    header.planes       = 1;
    header.bits         = bits;
    header.data_size    = scanline * channel->height;
    header.xppm         = 2834; // 72dpi
    header.yppm         = 2834; // 72dpi
    header.colors       = 1 << bits;
    header.important    = palette->size;

    file.offset        += header.colors * 4;
    file.size           = file.offset + header.data_size;

    FILE * f = fopen(filename, "wb");
    if(!f) return 0;

    unsigned short id = 0x4D42;
    fwrite(&id, 2, 1, f);
    fwrite(&file, sizeof(struct xbitmap_file), 1, f);
    fwrite(&header, sizeof(struct xbitmap_header), 1, f);

    
    fclose(f);

    return 1;
}

int xbitmap_save(struct ximg * image, uint16_t index, const char * filename, uint8_t bits){
    if(bits > 8){
        return xbitmap_raster_save(image, index, filename, bits);
    }

    return xbitmap_mapped_save(image, index, filename, bits);
}