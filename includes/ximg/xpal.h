#ifndef XPAL_H
#define XPAL_H

/**
 * xpal a palette of colors
 */

#include <ximg/ximg.h>
#include <ximg/xtypes.h>

struct xpal {
    ximgtype_t type;
    uint16_t size;
} __attribute__((__packed__));

#define XPAL_RGB8 0x38424752
#define XPAL_RGBA 0x41424752
#define XPAL_VECT 0x54434556
#define XPAL_VEC1 0x31434556

/**
 * Create an palette inside the ximg
 *
 * Posible types are:
 * RGB8 - RGB 24bits
 * RGBA - RGB 32bits with alpha
 * VECT - RGB scale/value
 */
ximgid_t xpal_create(struct ximg * image, ximgtype_t type, uint16_t size);
uint32_t xpal_size(ximgtype_t type, uint16_t size);
struct xpal * xpal_get_by_index(struct ximg * image, uint16_t index);
struct xpal * xpal_get_by_id(struct ximg * image, ximgid_t id);

int xpal_get_rgb(struct xpal * palette, uint16_t index, struct xpixel * pixel);
int xpal_get_rgba(struct xpal * palette, uint16_t index, struct xpixel * pixel);
int xpal_get_vector(struct xpal * palette, uint16_t index, struct xvector * vector);
int xpal_get_vector1(struct xpal * palette, uint16_t index, struct xvector * vector);

int xpal_set_rgb(struct xpal * palette, uint16_t index, struct xpixel * pixel);
int xpal_set_rgba(struct xpal * palette, uint16_t index, struct xpixel * pixel);
int xpal_set_vector(struct xpal * palette, uint16_t index, struct xvector * vector);
int xpal_set_vector1(struct xpal * palette, uint16_t index, struct xvector * vector);
#endif
