#include <ximg/ximg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct ximg * ximg_create(){
    struct ximg * image = malloc(sizeof(struct ximg));
    if(!image) return 0;
    memset(image, 0, sizeof(struct ximg));
    memcpy(image->header.fileid, "Ximg\r\n10", 8);
    image->header.size = sizeof(struct ximg_header);
    return image;
}

void ximg_free(struct ximg * image){
    if(!image) return;
    
    if(!image->header.chunks){
        free(image);
        return;
    }

    int i = 0;
    while(i < image->header.chunks){
        if(!image->chunks[i]) continue;

        free(image->chunks[i]);
        i++;
    }
    free(image->chunks);
    free(image);
}

unsigned int ximg_add(struct ximg * image, unsigned int size, unsigned int type){
    struct xchu * chunk = malloc(sizeof(struct xchu) + size);
    if(!chunk) return 0;

    memset(chunk, 0, sizeof(struct xchu) + size);
    chunk->size = sizeof(struct xchu) + size;
    chunk->type = type;

    struct xchu ** chunks = malloc(sizeof(struct xchu*) * (image->header.chunks + 1));
    if(!chunks){
        free(chunk);
        return 0;
    }

    if(image->chunks){
        memcpy(chunks, image->chunks, sizeof(struct xchu*) * (image->header.chunks));
        free(image->chunks);
        image->chunks = chunks;

        unsigned int index = 0;
        unsigned int id = 0;
        while(index < image->header.chunks){
            if(image->chunks[index]->id > id) id = image->chunks[index]->id;
            index++;
        }
        chunk->id = id + 1;
    }else{
        image->chunks = chunks;
        chunk->id = 1;
    }

    chunks[image->header.chunks] = chunk;

    image->header.chunks++;
    image->header.size+= chunk->size;
    return chunk->id;
}

struct xchu * ximg_get(struct ximg * image, unsigned int id){
    unsigned short index = 0;
    while(index < image->header.chunks){
        if(image->chunks[index]->id == id) return image->chunks[index];
        index++;
    }
    return 0;
}

struct xchu * ximg_find(struct ximg * image, unsigned int type, unsigned short offset){
    unsigned short index = 0;
    while(index < image->header.chunks){
        if(image->chunks[index]->type == type){
            if(offset-- == 0){
                return image->chunks[index];
            }
        }
        index++;
    }
    return 0;
}

unsigned int ximg_make(const char * type){
    unsigned int value = 0;
    int length = (!type[0]) ? 0 : (!type[1]) ? 1 : (!type[2]) ? 2 : (!type[3]) ? 3 : 4;
    memcpy(&value, type, length);
    return value;
}

void * xchu_contents(struct xchu * chunk){
    if(!chunk) return 0;
    return ((void*)chunk) + sizeof(struct xchu);
}
