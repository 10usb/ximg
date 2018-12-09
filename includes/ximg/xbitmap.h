#ifndef XBITMAP_H
#define XBITMAP_H

/**
 * Main header file for libxbitmap library.
 */

#include <ximg/ximg.h>

/**
 * Tries to load a bitmap reading a raster image or a mapped image.
 * @param   filename  The location the bitmap can be found
 * @return a handle to a ximg on success or null
 */
struct ximg * xbitmap_load(const char * filename);

/**
 * To save content from an Ximg file to an bitmap
 * @param image     The Xing image to get the data from
 * @oaram index     Index of the raster or mapped image source withing the Xing handle
 * @param filename  The location the bitmap file need to be saved
 * @param bits      The number of bits to be used for a single pixel, posible values 1-8, 24, 32
 */
int xbitmap_save(struct ximg * image, uint16_t index, const char * filename, uint8_t bits);

#endif
