#include <ximg/xtranslate.h>
#include <ximg/xras.h>
#include <ximg/xmap.h>

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

ximgid_t xtranslate_raster(struct ximg * src, uint16_t index, struct ximg * dest){
    return 0;
}

ximgid_t xtranslate_raster_with_palette(struct ximg * src, uint16_t index, struct ximg * dest, uint16_t palette){
    return 0;
}
