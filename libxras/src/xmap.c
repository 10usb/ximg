#include <ximg/xmap.h>
#include <stdio.h>

unsigned int xmap_create(struct ximg * image, unsigned int width, unsigned int height, unsigned int type, unsigned short colors){
    return 0;
}

unsigned int xmap_create_with_palette(struct ximg * image, unsigned int width, unsigned int height, unsigned int palette){
    struct xpal * pal = xpal_get_by_id(image, palette);
    if(!pal) return 0;

    unsigned char bits = 8;
    if((pal->size - 1) > 0xff){
        bits = 16;
    }

    unsigned int id = ximg_add(image, sizeof(struct xmap), ximg_make("XMAP"));
    struct xmap * map = xmap_get_by_id(image, id);
    map->palette = palette;

    map->channel = ximg_add(image, xchan_size(width, height, bits), ximg_make("XCHA"));
    xchan_create(width, height, bits, xchu_contents(ximg_get(image, map->channel)));

    return id;
}

struct xmap * xmap_get_by_index(struct ximg * image, unsigned short index){
    struct xchu * chunk = ximg_find(image, ximg_make("XMAP"), index);
    if(!chunk) return 0;
    return xchu_contents(chunk);
}

struct xmap * xmap_get_by_id(struct ximg * image, unsigned int id){
    return xchu_contents(ximg_get(image, id));
}

struct xpal * xmap_palette(struct ximg * image, struct xmap * mapped){
    return xpal_get_by_id(image, mapped->palette);
}

struct xchan * xmap_channel(struct ximg * image, struct xmap * mapped){
    return xchu_contents(ximg_get(image, mapped->channel));
}