#include <ximg/xchan.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct xchan * xchan_create(unsigned int width, unsigned int height, unsigned char bits, void * data){
    if(!data) return 0;

    memset(data, 0, xchan_size(width, height, bits));

    struct xchan * chan = data;
    chan->width     = width;
    chan->height    = height;
    chan->bits      = bits;
    return chan;
}

inline unsigned int xchan_size(unsigned int width, unsigned int height, unsigned char bits){
    unsigned int line = width * (bits % 8);
    if(line % 8){
        line = (line / 8) + 1 + width * (bits / 8);
    }else{
        line = (line / 8) + width * (bits / 8);
    }
    return line * height + sizeof(struct xchan);
}

unsigned int xchan_sizeof(struct xchan * chan){
    return xchan_size(chan->width, chan->height, chan->bits);
}

unsigned char xchan_get8(struct xchan * chan, unsigned int x, unsigned int y){
    if(chan->bits!=8 || x>=chan->width || y>=chan->height) return 0;
    unsigned char * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

unsigned short xchan_get16(struct xchan * chan, unsigned int x, unsigned int y){
    if(chan->bits!=16 || x>=chan->width || y>=chan->height) return 0;
    unsigned short * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

unsigned int xchan_get32(struct xchan * chan, unsigned int x, unsigned int y){
    if(chan->bits!=32 || x>=chan->width || y>=chan->height) return 0;
    unsigned int * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

unsigned long long xchan_get64(struct xchan * chan, unsigned int x, unsigned int y){
    if(chan->bits!=64 || x>=chan->width || y>=chan->height) return 0;
    unsigned long long * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

float xchan_getf(struct xchan * chan, unsigned int x, unsigned int y){
    if(chan->bits!=(sizeof(float) * 8) || x>=chan->width || y>=chan->height) return 0;
    float * data = ((void*)chan) + sizeof(struct xchan);
    return data[x + y * chan->width];
}

void xchan_set8(struct xchan * chan, unsigned int x, unsigned int y, unsigned char value){
    if(chan->bits!=8 || x>=chan->width || y>=chan->height) return;
    unsigned char * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_set16(struct xchan * chan, unsigned int x, unsigned int y, unsigned short value){
    if(chan->bits!=16 || x>=chan->width || y>=chan->height) return;
    unsigned short * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_set32(struct xchan * chan, unsigned int x, unsigned int y, unsigned int value){
    if(chan->bits!=32 || x>=chan->width || y>=chan->height) return;
    unsigned int * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_set64(struct xchan * chan, unsigned int x, unsigned int y, unsigned long long value){
    if(chan->bits!=64 || x>=chan->width || y>=chan->height) return;
    unsigned long long * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
void xchan_setf(struct xchan * chan, unsigned int x, unsigned int y, float value){
    if(chan->bits!=(sizeof(float) * 8) || x>=chan->width || y>=chan->height) return;
    float * data = ((void*)chan) + sizeof(struct xchan);
    data[x + y * chan->width] = value;
}
