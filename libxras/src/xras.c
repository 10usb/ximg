#include <ximg/xras.h>
#include <ximg/xchan.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

inline unsigned int xras_size(unsigned short channels){
    return sizeof(struct xras) + sizeof(unsigned int) * channels;
}

unsigned int xras_create(struct ximg * image, unsigned int width, unsigned int height, unsigned int type, unsigned short channels){
    unsigned char bits;
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

    unsigned int id = ximg_add(image, xras_size(channels), ximg_make("XRAS"));

    struct xras * raster = xchu_contents(ximg_get(image, id));

    unsigned int * identifiers = ((void*)raster) + sizeof(struct xras);

    raster->type = type;
    raster->channels = channels;
    for(int index = 0;  index < channels; index++){
        identifiers[index] = ximg_add(image, xchan_size(width, height, 8), ximg_make("XCHA"));
        xchan_create(width, height, 8, xchu_contents(ximg_get(image, identifiers[index])));
    }

    return id;
}

struct xras * xras_get_by_index(struct ximg * image, unsigned short index){
    struct xchu * chunk = ximg_find(image, ximg_make("XRAS"), index);
    if(!chunk) return 0;
    return (struct xras *)xchu_contents(chunk);
}

struct xras * xras_get_by_id(struct ximg * image, unsigned int id){
    struct xchu * chunk = ximg_get(image, id);
    if(!chunk) return 0;
    if(chunk->type != ximg_make("XRAS")) return 0;
    return (struct xras *)xchu_contents(chunk);
}

struct xchan * xras_channel(struct ximg * image, struct xras * raster, unsigned short channel){
    if(channel >= raster->channels) return 0;

    unsigned int * identifiers = ((void*)raster) + sizeof(struct xras);

    struct xchu * chunk = ximg_get(image, identifiers[channel]);
    if(!chunk) return 0;

    return (struct xchan *)xchu_contents(chunk);
}
