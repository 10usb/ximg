#include <ximg/ximg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct ximg * ximg_load(const char * filename){
    FILE * f = fopen(filename, "rb");
    if(!f) return 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(size < sizeof(struct ximg_header)){
        fclose(f);
        return 0;
    }

    struct ximg * image = malloc(sizeof(struct ximg));
    memset(image, 0, sizeof(struct ximg));

    if(!fread(&image->header, sizeof(struct ximg_header), 1, f)){
        free(image);
        fclose(f);
        return 0;
    }

    if(memcmp(image->header.fileid, "Ximg\r\n10", 8) != 0){
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
    if(!image->chunks){
        free(image);
        fclose(f);
        return 0;
    }

    int index = 0;
    while(index < image->header.chunks){
        uint32_t chunk_size;

        // Read total size of chunk
        fread(&chunk_size, sizeof(uint32_t), 1, f);

        // Allocate chunk & set size
        struct xchu * chunk = (struct xchu*)malloc(chunk_size);
        chunk->size = chunk_size;

        // Read remaining data of chunk
        fread(((void*)chunk) + sizeof(uint32_t), 1, chunk_size - sizeof(uint32_t), f);

        image->chunks[index] = chunk;
        index++;
    }
    fclose(f);

    return image;
}

int ximg_save(struct ximg * image, const char * filename){
    if(!image) return 0;
    FILE * f = fopen(filename, "wb");
    if(!f) return 0;

    fwrite(&image->header, 1, sizeof(struct ximg_header), f);
    int index = 0;
    while(index < image->header.chunks){
        fwrite(image->chunks[index], 1, image->chunks[index]->size, f);
        index++;
    }
    fclose(f);

    return 1;
}