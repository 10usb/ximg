#ifndef XANIMATE_H
#define XANIMATE_H

/**
 * Animation
 */

#include <ximg/ximg.h>

struct xanimate {
    uint16_t width;
    uint16_t height;
    uint32_t frames;
} __attribute__((__packed__));

struct xframe {
    uint16_t left;
    uint16_t top;
    uint32_t delay : 24;
    ximgid_t source;
    uint8_t mode;
} __attribute__((__packed__));

ximgid_t xanimate_create(struct ximg * image, uint16_t width, uint16_t height, uint32_t frames);
struct xanimate * xanimate_get_by_index(struct ximg * image, uint16_t index);
struct xanimate * xanimate_get_by_id(struct ximg * image, ximgid_t id);
struct xframe * xanimate_frames(struct xanimate * animate);
#endif
