#ifndef XRAS_H
#define XRAS_H

/*
 * xras a raster image
 */

#include <ximg/ximg.h>
#include <ximg/xchan.h>

struct xras {
    unsigned int type;
    unsigned short channels;
} __attribute__((__packed__));


#define XRAS_RGB8 0x38424752
#define XRAS_RGBA 0x41424752
#define XRAS_VECT 0x54434556
#define XRAS_VEC1 0x31434556

/**
 * Create an raster image inside de ximg
 *
 * Posible types are:
 * RGB8 - RGB 24bits image
 * RGBA - RGB 32bits with alpha
 * VECT - RGB scale/value
 */
unsigned int xras_create(struct ximg * image, unsigned int width, unsigned int height, unsigned int type, unsigned short channels);
unsigned int xras_size(unsigned short channels);
struct xras * xras_get_by_index(struct ximg * image, unsigned short index);
struct xras * xras_get_by_id(struct ximg * image, unsigned int id);
struct xchan * xras_channel(struct ximg * image, struct xras * raster, unsigned short channel);
#endif
