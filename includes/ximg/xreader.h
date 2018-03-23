#ifndef XREADER_H
#define XREADER_H

/*
 * Reader of raster images
 */

#include <ximg/ximg.h>
#include <ximg/xras.h>
#include <ximg/xchan.h>
#include <ximg/xtypes.h>

struct xreader {
    unsigned short width;
    unsigned short height;
    struct ximg * image;
    struct xras * raster;
    struct xchan ** channnels;
};

int xreader_init(struct xreader * reader, struct ximg * image, unsigned short index);
void xreader_clear(struct xreader * reader);

int xreader_rgb(struct xreader * reader, unsigned short x, unsigned short y, struct xpixel * pixel);
int xreader_rgba(struct xreader * reader, unsigned short x, unsigned short y, struct xpixel * pixel);
int xreader_vector(struct xreader * reader, unsigned short x, unsigned short y, struct xvector * vector);
int xreader_vector1(struct xreader * reader, unsigned short x, unsigned short y, struct xvector * vector);
#endif
