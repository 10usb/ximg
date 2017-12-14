#include <ximg/ximg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct ximg * ximg_load(const char * filename){
    FILE * f = fopen(filename, "rb");
    if(!f) return 0;

    fseek(f, 0, SEEK_END);
    unsigned int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(size < sizeof(struct ximg_header)){
        fclose(f);
        return 0;
    }

    struct ximg * image = (struct ximg*)malloc(sizeof(struct ximg));
    memset(image, 0, sizeof(struct ximg));

    fread(&image->header, 1, sizeof(struct ximg_header), f);

    if(memcmp(image->header.fileid, "Ximg\r\n10", 8)!=0){
        free(image);
        fclose(f);
        return 0;
    }

    if(image->header.size > size){
        free(image);
        fclose(f);
        return 0;
    }

    image->chunks = malloc(sizeof(struct xchu*) * (image->header.chunks));

    int i = 0;
    while(i<image->header.chunks){
        unsigned int chunk_size;
        fread(&chunk_size, sizeof(unsigned int), 1, f);
        struct xchu * chunk = (struct xchu*)malloc(chunk_size);
        chunk->size = chunk_size;
        fread(((void*)chunk) + sizeof(unsigned int), 1, chunk->size - sizeof(unsigned int), f);

        image->chunks[i] = chunk;
        i++;
    }
    fclose(f);

    return image;
}

int ximg_save(struct ximg * image, const char * filename){
    FILE * f = fopen(filename, "wb");
    if(!f) return 0;

    fwrite(&image->header, 1, sizeof(struct ximg_header), f);
    int i = 0;
    while(i<image->header.chunks){
        fwrite(image->chunks[i], 1, image->chunks[i]->size, f);
        i++;
    }
    fclose(f);

    return 1;
}