#ifndef XTYPES_H
#define XTYPES_H

struct xpixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} __attribute__((__packed__));

struct xvector {
    float r;
    float g;
    float b;
    float v;
    float a;
} __attribute__((__packed__));

#endif
