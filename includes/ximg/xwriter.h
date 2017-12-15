#ifndef XWRITER_H
#define XWRITER_H

/*
 * Writer of raster images
 */

#include <ximg/xreader.h>

struct xwriter {
    unsigned short width;
    unsigned short height;
    unsigned int type;
    struct ximg * image;
    struct xras * raster;
    struct xchan ** channnels;
};

int xwriter_create(struct xwriter * writer, struct ximg * image, unsigned short width, unsigned short height, unsigned int type);
int xwriter_open(struct xwriter * writer, struct ximg * image, unsigned int id);
int xwriter_copy(struct xwriter * writer, struct xwriter * reader);
void xwriter_clear(struct xwriter * writer);
#endif
