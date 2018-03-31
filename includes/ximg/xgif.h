#ifndef XGIF_H
#define XGIF_H

/**
 * Handles the GIF format
 * (GIF in dutch has a so much cooler meaning)
 */

#include <ximg/ximg.h>

/**
 * Load a gif file
 */
struct ximg * xgif_load(const char * filename);

/**
 * Save an mapped image or an animation with only mapped images
 */
int xgif_save(struct ximg * image, ximgid_t id, const char * filename);

#endif
