#ifndef XWRITER_H
#define XWRITER_H

/**
 * Writer of raster images
 */

#include <ximg/xreader.h>

struct xwriter {
    uint16_t width;
    uint16_t height;
    ximgtype_t type;
    struct ximg * image;
    struct xras * raster;
    struct xchan ** channnels;
};

int xwriter_create(struct xwriter * writer, struct ximg * image, uint16_t width, uint16_t height, ximgtype_t type);
int xwriter_open(struct xwriter * writer, struct ximg * image, uint16_t index);
int xwriter_copy(struct xwriter * writer, struct xreader * reader);
void xwriter_clear(struct xwriter * writer);
#endif
