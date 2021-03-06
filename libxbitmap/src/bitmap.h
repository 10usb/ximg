#ifndef BITMAP_H
#define BITMAP_H

/**
 * Used in the .c file of this library to ease the handling of the bitmap file
 */

#include <stdint.h>
#include <stdio.h>

struct xbitmap_file {
  uint32_t size;            // (4 bytes),
  uint32_t reserved;        // (4 bytes),
  uint32_t offset;          // (4 bytes), offset to the data
} __attribute__((__packed__));

struct xbitmap_header {
  uint32_t size;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bits;      // 1,4,8,24,32
  uint32_t compression;
  uint32_t data_size;
  uint32_t xppm;
  uint32_t yppm;
  uint32_t colors;
  uint32_t important;
} __attribute__((__packed__));

struct xbitmap_rgba {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t a;
} __attribute__((__packed__));

struct bitmap_stream {
    uint32_t offset;
    int bits;
    uint32_t scanline;
    int position;
    uint8_t buffer;
    uint8_t mask;
};

void bitmap_stream_init(struct bitmap_stream * stream, uint32_t offset, int bits, uint32_t width);

void bitmap_stream_align_read(struct bitmap_stream * stream, FILE * f);
int bitmap_stream_read(struct bitmap_stream * stream, FILE * f, uint8_t * value);

int bitmap_stream_align_write(struct bitmap_stream * stream, FILE * f);
int bitmap_stream_write(struct bitmap_stream * stream, FILE * f, uint8_t value);

#endif
