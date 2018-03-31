#ifndef XTYPES_H
#define XTYPES_H

#include <ximg/ximg.h>

/**
 * A RGB color structure with alpha support
 */
struct xpixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} __attribute__((__packed__));

#define XPIXEL_RGB8 0x38424752 // RGB8 in ASCII
#define XPIXEL_RGBA 0x41424752 // RGBA in ASCII

#define XPIXEL_RGB8_SIZE (sizeof(uint8_t) * 3)
#define XPIXEL_RGBA_SIZE (sizeof(uint8_t) * 4)

/**
 * A RGB & Value color structure with alpha support
 * 
 * The values can be in 2 formats
 *  - The RGB values are scales to value
 *  - The RGB values are in balance, thus sum(RGB) = 1
 */
struct xvector {
    float r;
    float g;
    float b;
    float v;
    float a;
} __attribute__((__packed__));

#define XVECTOR_VECT 0x54434556 // VECT in ASCII
#define XVECTOR_VEC1 0x31434556 // VEC1 in ASCII

#define XVECTOR_VECT_SIZE (sizeof(float) * 4)
#define XVECTOR_VEC1_SIZE (sizeof(float) * 4)

/**
 * Converts a supported type to an other supported type
 * @return 1 of success, 0 on failure
 */
int xtypes_convert(const void * src, void * dest, ximgtype_t src_type, ximgtype_t desc_type);
#endif
