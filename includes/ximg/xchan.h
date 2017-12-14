#ifndef XCHAN_H
#define XCHAN_H

/**
 * Channel of a raster image
 */
struct xchan {
    unsigned short width;
    unsigned short height;
    unsigned char bits;
} __attribute__((__packed__ ));

/**
 * Creates a new channel stucture at the given data
 */
struct xchan * xchan_create(unsigned int width, unsigned int height, unsigned char bits, void * data);
unsigned int xchan_size(unsigned int width, unsigned int height, unsigned char bits);
unsigned int xchan_sizeof(struct xchan * chan);

unsigned char xchan_get8(struct xchan * chan, unsigned int x, unsigned int y);
unsigned short xchan_get16(struct xchan * chan, unsigned int x, unsigned int y);
unsigned int xchan_get32(struct xchan * chan, unsigned int x, unsigned int y);
unsigned long long xchan_get64(struct xchan * chan, unsigned int x, unsigned int y);
float xchan_getf(struct xchan * chan, unsigned int x, unsigned int y);

void xchan_set8(struct xchan * chan, unsigned int x, unsigned int y, unsigned char value);
void xchan_set16(struct xchan * chan, unsigned int x, unsigned int y, unsigned short value);
void xchan_set32(struct xchan * chan, unsigned int x, unsigned int y, unsigned int value);
void xchan_set64(struct xchan * chan, unsigned int x, unsigned int y, unsigned long long value);
void xchan_setf(struct xchan * chan, unsigned int x, unsigned int y, float value);
#endif
