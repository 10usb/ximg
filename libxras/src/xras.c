#include <ximg/xras.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

inline uint32_t xras_size(uint8_t channels){
    return sizeof(struct xras) + sizeof(ximgid_t) * channels;
}

inline ximgid_t * xras_channel_ids(struct xras * raster){
    if(!raster) return 0;
    return ((void*)raster) + sizeof(struct xras);
}

ximgid_t xras_create(struct ximg * image, uint16_t width, uint16_t height, ximgtype_t type, uint8_t channels){
    uint8_t bits;
    // RGB8 - RGB 24bits image
    // RGBA - RGB 32bits with alpha
    // VECT - RGB scale/value
    if(ximg_make("RGB8") == type){
        channels = 3;
        bits = 8;
    }else if(ximg_make("RGBA") == type){
        channels = 4;
        bits = 8;
    }else if(ximg_make("VECT") == type || ximg_make("VEC1") == type){
        channels = 4;
        bits = sizeof(float) * 8;
    }else{
        return 0;
    }

    ximgid_t id = ximg_add(image, xras_size(channels), ximg_make("XRAS"));
    if(!id) return 0;

    struct xras * raster = xchu_contents(ximg_get(image, id));
    if(!raster) return 0;

    ximgid_t * identifiers = xras_channel_ids(raster);

    raster->type = type;
    raster->channels = channels;
    for(int index = 0;  index < channels; index++){
        identifiers[index] = ximg_add(image, xchan_size(width, height, bits), ximg_make("XCHA"));
        if(!identifiers[index]){
            // TODO remove XRAS chunk
            return 0;
        }
        xchan_create(width, height, bits, xchu_contents(ximg_get(image, identifiers[index])));
    }

    return id;
}

struct xras * xras_get_by_index(struct ximg * image, uint16_t index){
    return xchu_contents(ximg_find(image, ximg_make("XRAS"), index));
}

struct xras * xras_get_by_id(struct ximg * image, ximgid_t id){
    struct xchu * chunk = ximg_get(image, id);
    if(!chunk) return 0;
    if(chunk->type != ximg_make("XRAS")) return 0;
    return xchu_contents(chunk);
}

struct xchan * xras_channel(struct ximg * image, struct xras * raster, uint8_t channel){
    if(!raster || channel >= raster->channels) return 0;

    ximgid_t * identifiers = xras_channel_ids(raster);
    if(!identifiers) return 0;

    struct xchu * chunk = ximg_get(image, identifiers[channel]);
    if(!chunk) return 0;

    return xchu_contents(chunk);
}
