#ifndef XRAS_H
#define XRAS_H

/*
 * xras a raster image
 */

#include <ximg/ximg.h>
#include <ximg/xchan.h>

struct xras {
    ximgtype_t type;
    uint8_t channels;
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
ximgid_t xras_create(struct ximg * image, uint16_t width, uint16_t height, ximgtype_t type, uint8_t channels);
uint32_t xras_size(uint8_t channels);
struct xras * xras_get_by_index(struct ximg * image, uint16_t index);
struct xras * xras_get_by_id(struct ximg * image, ximgid_t id);
ximgid_t * xras_channel_ids(struct xras * raster);
struct xchan * xras_channel(struct ximg * image, struct xras * raster, uint8_t channel);
#endif
