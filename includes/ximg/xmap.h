#ifndef XMAP_H
#define XMAP_H

/*
 * xmap a mapped raster image
 */

#include <ximg/ximg.h>
#include <ximg/xchan.h>
#include <ximg/xpal.h>

struct xmap {
    unsigned int palette;
    unsigned int channel;
} __attribute__((__packed__));

/**
 * Create an mapped image inside de ximg
 *
 * Posible types are:
 * RGB8 - RGB 24bits image
 * RGBA - RGB 32bits with alpha
 * VECT - RGB scale/value
 */
unsigned int xmap_create(struct ximg * image, unsigned int width, unsigned int height, unsigned int type, unsigned short colors);
unsigned int xmap_create_with_palette(struct ximg * image, unsigned int width, unsigned int height, unsigned int palette);
struct xmap * xmap_get_by_index(struct ximg * image, unsigned short index);
struct xmap * xmap_get_by_id(struct ximg * image, unsigned int id);
struct xpal * xmap_palette(struct ximg * image, struct xmap * mapped);
struct xchan * xmap_channel(struct ximg * image, struct xmap * mapped);
#endif
