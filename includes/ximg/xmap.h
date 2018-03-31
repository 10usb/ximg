#ifndef XMAP_H
#define XMAP_H

/*
 * xmap a mapped raster image
 */

#include <ximg/ximg.h>
#include <ximg/xchan.h>
#include <ximg/xpal.h>

struct xmap {
    ximgid_t palette;
    ximgid_t channel;
} __attribute__((__packed__));

/**
 * Create an mapped image inside de ximg
 *
 * Posible types are:
 * RGB8 - RGB 24bits image
 * RGBA - RGB 32bits with alpha
 * VECT - RGB scale/value
 */
ximgid_t xmap_create(struct ximg * image, uint16_t width, uint16_t height, uint16_t type, uint16_t colors);
ximgid_t xmap_create_with_palette(struct ximg * image, uint16_t width, uint16_t height, ximgid_t palette);
struct xmap * xmap_get_by_index(struct ximg * image, uint16_t index);
struct xmap * xmap_get_by_id(struct ximg * image, ximgid_t id);
struct xpal * xmap_palette(struct ximg * image, struct xmap * mapped);
struct xchan * xmap_channel(struct ximg * image, struct xmap * mapped);
#endif
