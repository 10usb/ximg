#ifndef XBITMAP_H
#define XBITMAP_H

/**
 * Handles the bitmap format
 */

#include <ximg/ximg.h>

struct ximg * xbitmap_load(const char * filename);
int xbitmap_save(struct ximg * image, unsigned short index, const char * filename, unsigned char bits);

#endif
