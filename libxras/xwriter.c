#include <ximg/xwriter.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int xwriter_create(struct xwriter * writer, struct ximg * image, unsigned short width, unsigned short height, unsigned int type){
    writer->image   = image;
    writer->width   = width;
    writer->height  = height;
    writer->type    = type;

    struct xchu * info = ximg_find(image, ximg_make("XINF"), 0);
    if(info){
        return 0; // not supported yet
    }
    puts("Creating.......");
    return 1;
}

int xwriter_open(struct xwriter * writer, struct ximg * image, unsigned int id){
    writer->image = image;

}

int xwriter_copy(struct xwriter * writer, struct xwriter * reader){
    puts("Copying.......");
    return 0;
}

void xwriter_clear(struct xwriter * writer){
    
}