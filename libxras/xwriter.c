#include <ximg/xwriter.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int xwriter_create(struct xwriter * writer, struct ximg * image, unsigned short width, unsigned short height, unsigned int type){
    reader->image   = image;
    reader->width   = width;
    reader->height  = height;
    reader->type    = type;

    struct xchu * info = ximg_find(image, ximg_make("XINF"), 0);
    if(info){
        return 0; // not supported yet
    }
}

int xwriter_open(struct xwriter * writer, struct ximg * image, unsigned int id){
    reader->image = image;

}

int xwriter_copy(struct xwriter * writer, struct xwriter * reader){

}

void xreader_clear(struct xwriter * writer){
    
}