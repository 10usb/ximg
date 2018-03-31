#include <ximg/xchan.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct xchan * xchan_create(uint16_t width, uint16_t height, uint8_t bits, void * data){
    if(!data) return 0;

    memset(data, 0, xchan_size(width, height, bits));

    struct xchan * chan = data;
    chan->width     = width;
    chan->height    = height;
    chan->bits      = bits;
    return chan;
}

inline uint32_t xchan_size(uint16_t width, uint16_t height, uint8_t bits){
    uint32_t line = width * (bits % 8);
    if(line % 8){
        line = (line / 8) + 1 + width * (bits / 8);
    }else{
        line = (line / 8) + width * (bits / 8);
    }
    return line * height + sizeof(struct xchan);
}

uint32_t xchan_sizeof(struct xchan * chan){
    return xchan_size(chan->width, chan->height, chan->bits);
}

uint8_t xchan_get8(struct xchan * chan, uint16_t x, uint16_t y){
    if(!chan || chan->bits != 8 || x >= chan->width || y >= chan->height) return 0;
    unsigned char * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

uint16_t xchan_get16(struct xchan * chan,  uint16_t x, uint16_t y){
    if(!chan || chan->bits != 16 || x >= chan->width || y >= chan->height) return 0;
    unsigned short * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

uint32_t xchan_get32(struct xchan * chan,  uint16_t x, uint16_t y){
    if(!chan || chan->bits != 32 || x >= chan->width || y >= chan->height) return 0;
    unsigned int * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

uint64_t xchan_get64(struct xchan * chan,  uint16_t x, uint16_t y){
    if(!chan || chan->bits != 64 || x >= chan->width || y >= chan->height) return 0;
    unsigned long long * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

float xchan_getf(struct xchan * chan,  uint16_t x, uint16_t y){
    if(!chan || chan->bits != (sizeof(float) * 8) || x >= chan->width || y >= chan->height) return 0;
    float * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

void xchan_set8(struct xchan * chan,  uint16_t x, uint16_t y, uint8_t value){
    if(!chan || chan->bits != 8 || x >= chan->width || y >= chan->height) return;
    uint8_t * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_set16(struct xchan * chan,  uint16_t x, uint16_t y, uint16_t value){
    if(!chan || chan->bits != 16 || x >= chan->width || y >= chan->height) return;
    uint16_t * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_set32(struct xchan * chan,  uint16_t x, uint16_t y, uint32_t value){
    if(!chan || chan->bits != 32 || x >= chan->width || y >= chan->height) return;
    uint32_t * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_set64(struct xchan * chan,  uint16_t x, uint16_t y, uint64_t value){
    if(!chan || chan->bits != 64 || x >= chan->width || y >= chan->height) return;
    uint64_t * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_setf(struct xchan * chan,  uint16_t x, uint16_t y, float value){
    if(!chan || chan->bits != (sizeof(float) * 8) || x >= chan->width || y >= chan->height) return;
    float * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
