#include <ximg/xtranslate.h>
#include <ximg/xras.h>
#include <ximg/xmap.h>
#include <ximg/xreader.h>
#include <stdio.h>
#include <stdlib.h>

static inline void copy_mapped_rgb8(struct xchan * data, struct xpal * palette, struct ximg * image, struct xras * raster){
    struct xchan * channels[3];
    channels[0] = xras_channel(image, raster, 0);
    channels[1] = xras_channel(image, raster, 1);
    channels[2] = xras_channel(image, raster, 2);
    
    for(uint16_t y = 0; y < data->height; y++){
        for(uint16_t x = 0; x < data->width; x++){
            uint16_t index = xchan_get8(data, x, y);
            struct xpixel pixel;
            xpal_get_rgb(palette, index, &pixel);

            xchan_set8(channels[0], x, y, pixel.r);
            xchan_set8(channels[1], x, y, pixel.g);
            xchan_set8(channels[2], x, y, pixel.b);
        }
    }
}

/**
 * It extracts the palette and data channel of the mapped image
 * Create an rastor image with the pixel format the palette is in
 * For each index in the data channel
 *  - get the pixel or vector data
 *  - store it in the raster image
 */
ximgid_t xtranslate_mapped(struct ximg * src, ximgid_t id, struct ximg * dest){
    struct xmap * mapped = xmap_get_by_id(src, id);

    struct xchan * data = xmap_channel(src, mapped);
    struct xpal * palette =  xmap_palette(src, mapped);

    ximgid_t raster_id = xras_create(dest, data->width, data->height, palette->type, 0);

    struct xras * raster = xras_get_by_id(dest, raster_id);

    switch(palette->type){
        case XPIXEL_RGB8: copy_mapped_rgb8(data, palette, dest, raster); break;
        // TODO clean created raster image
        default: return 0;
    }

    return id;
}

ximgid_t xtranslate_raster(struct ximg * src, ximgid_t id, struct ximg * dest){
    return 0;
}


int xtranslate_reader_init(struct xreader * reader, struct ximg * image, ximgid_t id){
    reader->image = image;

    struct xchu * raster = ximg_get(image, id);
    if(!raster || raster->type!=ximg_make("XRAS")) return 0;

    reader->raster = xchu_contents(raster);

    reader->channnels = malloc(sizeof(struct xchan *) * reader->raster->channels);

    for(int channel = 0; channel < reader->raster->channels; channel++){
        reader->channnels[channel] = xras_channel(image, reader->raster, channel);
    }

    reader->width  = reader->channnels[0]->width;
    reader->height = reader->channnels[0]->height;
    return 1;
}

void xtranslate_apply_error(struct xpixel * pixel, struct xvector * error, struct xvector * remain){
    float r, g, b;
    r = pixel->r + error->r;
    g = pixel->g + error->g;
    b = pixel->b + error->b;

    remain->r = 0;
    remain->g = 0;
    remain->b = 0;

    if(r < 0){ r = 0; remain->r = r; }
    if(g < 0){ g = 0; remain->g = g; }
    if(b < 0){ b = 0; remain->b = b; }

    if(r > 255){ r = 255; remain->r = r - 255; }
    if(g > 255){ g = 255; remain->g = g - 255; }
    if(b > 255){ b = 255; remain->b = b - 255; }

    pixel->r = r;
    pixel->g = g;
    pixel->b = b;
}

void xtranslate_calculate_error(struct xpixel * target, struct xpixel * actual, struct xvector * error){
    error->r+= ((float)target->r) - ((float)actual->r);
    error->g+= ((float)target->g) - ((float)actual->g);
    error->b+= ((float)target->b) - ((float)actual->b);
}

void xtranslate_add_error(struct xvector * pixel, struct xvector * error, int part){
    pixel->r += error->r * part / 16;
    pixel->g += error->g * part / 16;
    pixel->b += error->b * part / 16;
}

ximgid_t xtranslate_raster_with_palette(struct ximg * src, ximgid_t id, struct ximg * dest, ximgid_t palette_id){
    // Init the deprecated reader with a custom loader
    struct xreader reader;
    if(!xtranslate_reader_init(&reader, src, id)) return 0;

    struct xpal * palette = xpal_get_by_id(dest, palette_id);
    if(!palette) return 0;

    ximgid_t mapped_id = xmap_create_with_palette(dest, reader.width, reader.height, palette_id);
    
    struct xmap * mapped = xmap_get_by_id(dest, mapped_id);
    
    struct xchan * channel = xmap_channel(dest, mapped);
    struct xpixel xpixel, pcolor;
    struct xvector * errors = calloc(reader.height * reader.width, sizeof(struct xvector));
    struct xvector error;

    for(int y = 0; y < reader.height; y++){
        for(int x = 0; x < reader.width; x++){
            xreader_rgb(&reader, x, y, &xpixel);
            xtranslate_apply_error(&xpixel, &errors[x + y * reader.height], &error);

            uint16_t index = xpal_match_rgb(palette, &xpixel);
            xchan_set8(channel, x, y, index);
            xpal_get_rgb(palette, index, &pcolor);

            xtranslate_calculate_error(&xpixel, &pcolor, &error);
            
            if((x + 1) <  reader.width) xtranslate_add_error(&errors[(x + 1) + y * reader.height], &error, 7);
            if((y + 1) <  reader.height){
                if((x - 1) > 0)            xtranslate_add_error(&errors[(x - 1) + (y + 1) * reader.height], &error, 3);
                                           xtranslate_add_error(&errors[(x    ) + (y + 1) * reader.height], &error, 5);
                if((x + 1) < reader.width) xtranslate_add_error(&errors[(x + 1) + (y + 1) * reader.height], &error, 1);
            }
        }
    }

    xreader_clear(&reader);
    return mapped_id;
}
