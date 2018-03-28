#include <ximg/xpal.h>
#include <string.h>

static inline int xtypes_copy(const struct xpixel * src, struct xpixel * dest, int size){
    memcpy(dest, src, size);
    return 1;
}

static inline int xpixel_rgb8_to_rgba(const struct xpixel * src, struct xpixel * dest){
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    dest->a = 255;
    return 1;
}

static inline int xpixel_rgba_to_rgb8(const struct xpixel * src, struct xpixel * dest){
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
    return 1;
}

int xtypes_convert(const void * src, void * dest, unsigned int src_type, unsigned int desc_type){
    switch(src_type){
        case XPIXEL_RGB8: {
            switch(desc_type){
                case XPIXEL_RGB8: return xtypes_copy(src, dest, XPIXEL_RGB8_SIZE);
                case XPIXEL_RGBA: return xpixel_rgb8_to_rgba(src, dest);
            }
        } break;
        case XPIXEL_RGBA: {
            switch(desc_type){
                case XPIXEL_RGB8: return xpixel_rgba_to_rgb8(src, dest);
                case XPIXEL_RGBA: return xtypes_copy(src, dest, XPIXEL_RGB8_SIZE);
            }
        } break;
    }
    return 0;
}