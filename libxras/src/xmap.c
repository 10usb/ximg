#include <ximg/xmap.h>
#include <stdio.h>

ximgid_t xmap_create(struct ximg * image, uint16_t width, uint16_t height, uint16_t type, uint16_t colors, int transparent){
    return 0;
}

ximgid_t xmap_create_with_palette(struct ximg * image, uint16_t width, uint16_t height, ximgid_t palette, int transparent){
    struct xpal * pal = xpal_get_by_id(image, palette);
    if(!pal) return 0;

    int bits = 8;
    if((pal->size - 1) > 0xff){
        bits = 16;
    }

    ximgid_t id = ximg_add(image, transparent ? sizeof(struct xmap) + sizeof(uint16_t): sizeof(struct xmap), ximg_make("XMAP"));
    if(!id) return 0;

    struct xmap * map = xmap_get_by_id(image, id);
    if(!map) return 0;

    map->palette = palette;
    map->channel = ximg_add(image, xchan_size(width, height, bits), ximg_make("XCHA"));

    if(!xchan_create(width, height, bits, xchu_contents(ximg_get(image, map->channel)))) return 0;

    return id;
}

struct xmap * xmap_get_by_index(struct ximg * image, uint16_t index){
    return xchu_contents(ximg_find(image, ximg_make("XMAP"), index));
}

struct xmap * xmap_get_by_id(struct ximg * image, ximgid_t id){
    struct xchu * chunk = ximg_get(image, id);
    if(!chunk) return 0;
    if(chunk->type != ximg_make("XMAP")) return 0;
    return xchu_contents(chunk);
}

struct xpal * xmap_palette(struct ximg * image, struct xmap * mapped){
    return xpal_get_by_id(image, mapped->palette);
}

struct xchan * xmap_channel(struct ximg * image, struct xmap * mapped){
    struct xchu * chunk = ximg_get(image, mapped->channel);
    if(!chunk) return 0;
    if(chunk->type != ximg_make("XCHA")) return 0;
    return xchu_contents(chunk);
}

int xmap_get_transparent(struct ximg * image, ximgid_t id, uint16_t * index){
    struct xchu * chunk = ximg_get(image, id);
    if(!chunk) return 0;
    if(chunk->type != ximg_make("XMAP")) return 0;
    if(chunk->size != sizeof(struct xmap) + sizeof(uint16_t)) return 0;

    *index = *((uint16_t*)(xchu_contents(chunk) + sizeof(struct xmap)));
    return 1;
}

int xmap_set_transparent(struct ximg * image, ximgid_t id, uint16_t index){
    struct xchu * chunk = ximg_get(image, id);
    if(!chunk) return 0;
    if(chunk->type != ximg_make("XMAP")) return 0;
    if(chunk->size != sizeof(struct xmap) + sizeof(uint16_t)) return 0;

    *((uint16_t*)(xchu_contents(chunk) + sizeof(struct xmap))) = index;
    return 1;
}