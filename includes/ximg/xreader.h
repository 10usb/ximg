#ifndef XREADER_H
#define XREADER_H

/*
 * Reader of raster images
 */

#include <ximg/ximg.h>
#include <ximg/xras.h>
#include <ximg/xtypes.h>

struct xreader {
    uint16_t width;
    uint16_t height;
    struct ximg * image;
    struct xras * raster;
    struct xchan ** channnels;
};

int xreader_init(struct xreader * reader, struct ximg * image, uint16_t index);
void xreader_clear(struct xreader * reader);

int xreader_rgb(struct xreader * reader, uint16_t x, uint16_t y, struct xpixel * pixel);
int xreader_rgba(struct xreader * reader, uint16_t x, uint16_t y, struct xpixel * pixel);
int xreader_vector(struct xreader * reader, uint16_t x, uint16_t y, struct xvector * vector);
int xreader_vector1(struct xreader * reader, uint16_t x, uint16_t y, struct xvector * vector);
#endif
