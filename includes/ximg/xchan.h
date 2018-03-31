#ifndef XCHAN_H
#define XCHAN_H

#include <ximg/ximg.h>

/**
 * Channel of a raster image
 */
struct xchan {
    uint16_t width;
    uint16_t height;
    uint8_t bits;
} __attribute__((__packed__ ));

/**
 * Creates a new channel stucture at the given data
 */
struct xchan * xchan_create(uint16_t width, uint16_t height, uint8_t bits, void * data);
uint32_t xchan_size(uint16_t width, uint16_t height, uint8_t bits);
uint32_t xchan_sizeof(struct xchan * chan);

uint8_t xchan_get8(struct xchan * chan, uint16_t x, uint16_t y);
uint16_t xchan_get16(struct xchan * chan, uint16_t x, uint16_t y);
uint32_t xchan_get32(struct xchan * chan, uint16_t x, uint16_t y);
uint64_t xchan_get64(struct xchan * chan, uint16_t x, uint16_t y);
float xchan_getf(struct xchan * chan, uint16_t x, uint16_t y);

void xchan_set8(struct xchan * chan, uint16_t x, uint16_t y, uint8_t value);
void xchan_set16(struct xchan * chan, uint16_t x, uint16_t y, uint16_t value);
void xchan_set32(struct xchan * chan, uint16_t x, uint16_t y, uint32_t value);
void xchan_set64(struct xchan * chan, uint16_t x, uint16_t y, uint64_t value);
void xchan_setf(struct xchan * chan, uint16_t x, uint16_t y, float value);
#endif
