#ifndef XPAL_H
#define XPAL_H

/*
 * xpal a palette of colors
 */

#include <ximg/ximg.h>
#include <ximg/xtypes.h>

struct xpal {
    unsigned int type;
    unsigned short size;
} __attribute__((__packed__));


#define XPAL_RGB8 0x38424752
#define XPAL_RGBA 0x41424752
#define XPAL_VECT 0x54434556
#define XPAL_VEC1 0x31434556

/**
 * Create an raster image inside de ximg
 *
 * Posible types are:
 * RGB8 - RGB 24bits
 * RGBA - RGB 32bits with alpha
 * VECT - RGB scale/value
 */
unsigned int xpal_create(struct ximg * image, unsigned int type, unsigned short size);
unsigned int xpal_size(unsigned int type, unsigned short size);
struct xpal * xpal_get_by_index(struct ximg * image, unsigned short index);
struct xpal * xpal_get_by_id(struct ximg * image, unsigned int id);

int xpal_rgb(struct xpal * palette, unsigned short index, struct xpixel * pixel);
int xpal_rgba(struct xpal * palette, unsigned short index, struct xpixel * pixel);
int xpal_vector(struct xpal * palette, unsigned short index, struct xvector * vector);
int xpal_vector1(struct xpal * palette, unsigned short index, struct xvector * vector);
#endif
