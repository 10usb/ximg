#ifndef XANIMATE_H
#define XANIMATE_H

/**
 * Animation
 */

#include <ximg/ximg.h>

struct xanimate {
    unsigned short width;
    unsigned short height;
    unsigned int frames;
} __attribute__((__packed__));

struct xframe {
    unsigned short left;
    unsigned short top;
    unsigned char mode;
    unsigned int delay : 24;
    unsigned int source;
} __attribute__((__packed__));

unsigned int xanimate_create(struct ximg * image, unsigned short width, unsigned short height, unsigned int frames);
struct xanimate * xanimate_get_by_index(struct ximg * image, unsigned short index);
struct xanimate * xanimate_get_by_id(struct ximg * image, unsigned int id);
struct xframe * xanimate_frames(struct xanimate * animate);
#endif
