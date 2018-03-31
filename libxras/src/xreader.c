#include <ximg/xreader.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int xreader_init(struct xreader * reader, struct ximg * image, uint16_t index){
    reader->image = image;

    struct xchu * info = ximg_find(image, ximg_make("XINF"), 0);
    if(info){
        return 0; // not supported yet
    }

    struct xchu * raster = ximg_find(image, ximg_make("XRAS"), index);
    if(!raster) return 0;

    reader->raster = xchu_contents(raster);

    reader->channnels = malloc(sizeof(struct xchan *) * reader->raster->channels);

    int channel = 0;
    while(channel < reader->raster->channels){
        reader->channnels[channel] = xras_channel(image, reader->raster, channel);
        channel++;
    }

    reader->width  = reader->channnels[0]->width;
    reader->height = reader->channnels[0]->height;
    return 1;
}

void xreader_clear(struct xreader * reader){
    free(reader->channnels);
}

int xreader_rgb(struct xreader * reader, uint16_t x, uint16_t y, struct xpixel * pixel){
    switch(reader->raster->type){
        case XRAS_RGB8:
            pixel->r = xchan_get8(reader->channnels[0], x, y);
            pixel->g = xchan_get8(reader->channnels[1], x, y);
            pixel->b = xchan_get8(reader->channnels[2], x, y);
        return 1;
        case XRAS_RGBA:{
            pixel->r = xchan_get8(reader->channnels[0], x, y);
            pixel->g = xchan_get8(reader->channnels[1], x, y);
            pixel->b = xchan_get8(reader->channnels[2], x, y);
        } return 1;
        case XRAS_VECT:{
            float v = xchan_getf(reader->channnels[3], x, y);

            pixel->r = xchan_getf(reader->channnels[0], x, y) * 255 * v;
            pixel->g = xchan_getf(reader->channnels[1], x, y) * 255 * v;
            pixel->b = xchan_getf(reader->channnels[2], x, y) * 255 * v;
        } return 1;
        case XRAS_VEC1:{
            struct xvector vector;
            vector.r = xchan_getf(reader->channnels[0], x, y);
            vector.g = xchan_getf(reader->channnels[1], x, y);
            vector.b = xchan_getf(reader->channnels[2], x, y);
            vector.v = xchan_getf(reader->channnels[3], x, y);
            float m = vector.r > vector.b ? vector.r : vector.b; m = vector.g > m ? vector.g : m;

            pixel->r = vector.r / m * 255 * vector.v;
            pixel->g = vector.g / m * 255 * vector.v;
            pixel->b = vector.b / m * 255 * vector.v;
        } return 1;
        default:
            pixel->r = x % 255;
            pixel->g = y % 255;
            pixel->b = x % 127 + y % 127;
        return 0;
    }
}

int xreader_rgba(struct xreader * reader, uint16_t x, uint16_t y, struct xpixel * pixel){
    switch(reader->raster->type){
        case XRAS_RGB8:
            pixel->r = xchan_get8(reader->channnels[0], x, y);
            pixel->g = xchan_get8(reader->channnels[1], x, y);
            pixel->b = xchan_get8(reader->channnels[2], x, y);
            pixel->a = 255;
        return 1;
        case XRAS_RGBA:
            pixel->r = xchan_get8(reader->channnels[0], x, y);
            pixel->g = xchan_get8(reader->channnels[1], x, y);
            pixel->b = xchan_get8(reader->channnels[2], x, y);
            pixel->a = xchan_get8(reader->channnels[3], x, y);
        return 1;
        case XRAS_VECT:{
            float v = xchan_getf(reader->channnels[3], x, y);

            pixel->r = xchan_getf(reader->channnels[0], x, y) * 255 * v;
            pixel->g = xchan_getf(reader->channnels[1], x, y) * 255 * v;
            pixel->b = xchan_getf(reader->channnels[2], x, y) * 255 * v;
            pixel->a = 255;
        } return 1;
        case XRAS_VEC1:{
            struct xvector vector;
            vector.r = xchan_getf(reader->channnels[0], x, y);
            vector.g = xchan_getf(reader->channnels[1], x, y);
            vector.b = xchan_getf(reader->channnels[2], x, y);
            vector.v = xchan_getf(reader->channnels[3], x, y);
            float m = vector.r > vector.b ? vector.r : vector.b; m = vector.g > m ? vector.g : m;

            pixel->r = vector.r / m * 255 * vector.v;
            pixel->g = vector.g / m * 255 * vector.v;
            pixel->b = vector.b / m * 255 * vector.v;
            pixel->a = 255;
        } return 1;
        default:
            pixel->r = x % 255;
            pixel->g = y % 255;
            pixel->b = x % 127 + y % 127;
            pixel->a = 255;
        return 0;
    }
}

int xreader_vector(struct xreader * reader, uint16_t x, uint16_t y, struct xvector * vector){
    switch(reader->raster->type){
        case XRAS_RGB8:
        case XRAS_RGBA:{
            float r = (float)xchan_get8(reader->channnels[0], x, y);
            float g = (float)xchan_get8(reader->channnels[1], x, y);
            float b = (float)xchan_get8(reader->channnels[2], x, y);

            float v = r > b ? r : b; v = g > v ? g : v;

            vector->r = r / v;
            vector->g = g / v;
            vector->b = b / v;
            vector->v = v / 255.0f;
        }return 1;
        default:
            vector->r = ((float)x) / ((float)y);
            vector->g = ((float)y) / ((float)x);
            vector->b = ((float)x) / ((float)y) * 0.5 + ((float)y) / ((float)x) * 0.5;
            vector->v = ((float)y) / ((float)x);
        return 0;
    }
}


int xreader_vector1(struct xreader * reader, uint16_t x, uint16_t y, struct xvector * vector){
    switch(reader->raster->type){
        case XRAS_RGB8:
        case XRAS_RGBA:{
            float r = (float)xchan_get8(reader->channnels[0], x, y);
            float g = (float)xchan_get8(reader->channnels[1], x, y);
            float b = (float)xchan_get8(reader->channnels[2], x, y);

            float v = r > b ? r : b; v = g > v ? g : v;
            float s = r / v + g / v + b / v;

            vector->r = r / v / s;
            vector->g = g / v / s;
            vector->b = b / v / s;
            vector->v = v / 255.0f;
        }return 1;
        default:
            vector->r = ((float)x) / ((float)y);
            vector->g = ((float)y) / ((float)x);
            vector->b = ((float)x) / ((float)y) * 0.5 + ((float)y) / ((float)x) * 0.5;
            vector->v = ((float)y) / ((float)x);
        return 0;
    }
}
