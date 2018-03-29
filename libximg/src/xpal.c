#include <ximg/xpal.h>

unsigned int xpal_create(struct ximg * image, unsigned int type, unsigned short size){
    unsigned int id = ximg_add(image, xpal_size(type, size), ximg_make("XPAL"));
    struct xpal * palette = xpal_get_by_id(image, id);
    palette->type = type;
    palette->size = size;
    return id;
}

unsigned int xpal_size(unsigned int type, unsigned short size){
    if(ximg_make("RGB8") == type){
        return sizeof(struct xpal) + size * XPIXEL_RGB8_SIZE;
    }else if(ximg_make("RGBA") == type){
        return sizeof(struct xpal) + size * XPIXEL_RGBA_SIZE;
    }else if(ximg_make("VECT") == type || ximg_make("VEC1") == type){
        return sizeof(struct xpal) + size * XVECTOR_VECT_SIZE;
    }else{
        return sizeof(struct xpal);
    }
}

struct xpal * xpal_get_by_index(struct ximg * image, unsigned short index){
    struct xchu * chunk = ximg_find(image, ximg_make("XPAL"), index);
    if(!chunk) return 0;
    return xchu_contents(chunk);
}

struct xpal * xpal_get_by_id(struct ximg * image, unsigned int id){
    return xchu_contents(ximg_get(image, id));
}

static inline void * xpal_entry(struct xpal * palette, unsigned short index, int size){
    return ((void*)palette) + sizeof(struct xpal) + index * size;
}

int xpal_get_rgb(struct xpal * palette, unsigned short index, struct xpixel * pixel){
    if(!palette || !pixel ||index >= palette->size) return 0;

    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGB8_SIZE), pixel, XPIXEL_RGB8, XPIXEL_RGB8);
        case XPAL_RGBA: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGBA_SIZE), pixel, XPIXEL_RGBA, XPIXEL_RGB8);
        case XPAL_VECT: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VECT_SIZE), pixel, XVECTOR_VECT, XPIXEL_RGB8);
        case XPAL_VEC1: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VEC1_SIZE), pixel, XVECTOR_VEC1, XPIXEL_RGB8);
    }

    return 0;
}

int xpal_get_rgba(struct xpal * palette, unsigned short index, struct xpixel * pixel){
    if(!palette || !pixel ||index >= palette->size) return 0;

    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGB8_SIZE), pixel, XPIXEL_RGB8, XPIXEL_RGBA);
        case XPAL_RGBA: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGBA_SIZE), pixel, XPIXEL_RGBA, XPIXEL_RGBA);
        case XPAL_VECT: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VECT_SIZE), pixel, XVECTOR_VECT, XPIXEL_RGBA);
        case XPAL_VEC1: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VEC1_SIZE), pixel, XVECTOR_VEC1, XPIXEL_RGBA);
    }
    
    return 0;
}

int xpal_get_vector(struct xpal * palette, unsigned short index, struct xvector * vector){
    if(!palette || !vector ||index >= palette->size) return 0;

    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGB8_SIZE), vector, XPIXEL_RGB8, XVECTOR_VECT);
        case XPAL_RGBA: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGBA_SIZE), vector, XPIXEL_RGBA, XVECTOR_VECT);
        case XPAL_VECT: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VECT_SIZE), vector, XVECTOR_VECT, XVECTOR_VECT);
        case XPAL_VEC1: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VEC1_SIZE), vector, XVECTOR_VEC1, XVECTOR_VECT);
    }
    
    return 0;
}

int xpal_get_vector1(struct xpal * palette, unsigned short index, struct xvector * vector){
    if(!palette || !vector ||index >= palette->size) return 0;

    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGB8_SIZE), vector, XPIXEL_RGB8, XVECTOR_VEC1);
        case XPAL_RGBA: return xtypes_convert(xpal_entry(palette, index, XPIXEL_RGBA_SIZE), vector, XPIXEL_RGBA, XVECTOR_VEC1);
        case XPAL_VECT: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VECT_SIZE), vector, XVECTOR_VECT, XVECTOR_VEC1);
        case XPAL_VEC1: return xtypes_convert(xpal_entry(palette, index, XVECTOR_VEC1_SIZE), vector, XVECTOR_VEC1, XVECTOR_VEC1);
    }
    
    return 0;
}

int xpal_set_rgb(struct xpal * palette, unsigned short index, struct xpixel * pixel){
    if(!palette || !pixel ||index >= palette->size) return 0;
    
    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(pixel, xpal_entry(palette, index, XPIXEL_RGB8_SIZE), XPIXEL_RGB8, XPIXEL_RGB8);
        case XPAL_RGBA: return xtypes_convert(pixel, xpal_entry(palette, index, XPIXEL_RGBA_SIZE), XPIXEL_RGB8, XPIXEL_RGBA);
        case XPAL_VECT: return xtypes_convert(pixel, xpal_entry(palette, index, XVECTOR_VECT_SIZE), XPIXEL_RGB8, XVECTOR_VECT);
        case XPAL_VEC1: return xtypes_convert(pixel, xpal_entry(palette, index, XVECTOR_VEC1_SIZE), XPIXEL_RGB8, XVECTOR_VEC1);
    }

    return 0;
}

int xpal_set_rgba(struct xpal * palette, unsigned short index, struct xpixel * pixel){
    if(!palette || !pixel ||index >= palette->size) return 0;

    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(pixel, xpal_entry(palette, index, XPIXEL_RGB8_SIZE), XPIXEL_RGBA, XPIXEL_RGB8);
        case XPAL_RGBA: return xtypes_convert(pixel, xpal_entry(palette, index, XPIXEL_RGBA_SIZE), XPIXEL_RGBA, XPIXEL_RGBA);
        case XPAL_VECT: return xtypes_convert(pixel, xpal_entry(palette, index, XVECTOR_VECT_SIZE), XPIXEL_RGBA, XVECTOR_VECT);
        case XPAL_VEC1: return xtypes_convert(pixel, xpal_entry(palette, index, XVECTOR_VEC1_SIZE), XPIXEL_RGBA, XVECTOR_VEC1);
    }

    return 0;
}

int xpal_set_vector(struct xpal * palette, unsigned short index, struct xvector * vector){
    if(!palette || !vector ||index >= palette->size) return 0;

    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(vector, xpal_entry(palette, index, XPIXEL_RGB8_SIZE), XVECTOR_VECT, XPIXEL_RGB8);
        case XPAL_RGBA: return xtypes_convert(vector, xpal_entry(palette, index, XPIXEL_RGBA_SIZE), XVECTOR_VECT, XPIXEL_RGBA);
        case XPAL_VECT: return xtypes_convert(vector, xpal_entry(palette, index, XVECTOR_VECT_SIZE), XVECTOR_VECT, XVECTOR_VECT);
        case XPAL_VEC1: return xtypes_convert(vector, xpal_entry(palette, index, XVECTOR_VEC1_SIZE), XVECTOR_VECT, XVECTOR_VEC1);
    }

    return 0;
}

int xpal_set_vector1(struct xpal * palette, unsigned short index, struct xvector * vector){
    if(!palette || !vector ||index >= palette->size) return 0;

    switch(palette->type){
        case XPAL_RGB8: return xtypes_convert(vector, xpal_entry(palette, index, XPIXEL_RGB8_SIZE), XVECTOR_VEC1, XPIXEL_RGB8);
        case XPAL_RGBA: return xtypes_convert(vector, xpal_entry(palette, index, XPIXEL_RGBA_SIZE), XVECTOR_VEC1, XPIXEL_RGBA);
        case XPAL_VECT: return xtypes_convert(vector, xpal_entry(palette, index, XVECTOR_VECT_SIZE), XVECTOR_VEC1, XVECTOR_VECT);
        case XPAL_VEC1: return xtypes_convert(vector, xpal_entry(palette, index, XVECTOR_VEC1_SIZE), XVECTOR_VEC1, XVECTOR_VEC1);
    }

    return 0;
}