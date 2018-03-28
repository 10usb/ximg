#ifndef XGIF_H
#define XGIF_H

/**
 * Handles the GIF format
 * (GIF in dutch has a so much cooler meaning)
 */

#include <ximg/ximg.h>

struct ximg * xgif_load(const char * filename);
int xgif_save(struct ximg * image, unsigned int id, const char * filename, unsigned char bits);

#endif
