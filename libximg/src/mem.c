#include <ximg/ximg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Extracts an image from a memory buffer
 */
struct ximg * ximg_extract(const void * source, uint32_t size){
    if(!source) return 0;

    if(size < sizeof(struct ximg_header)) return 0;

    struct ximg * image = malloc(sizeof(struct ximg));
    memset(image, 0, sizeof(struct ximg));

    memcpy(&image->header, source, sizeof(struct ximg_header));
    source+= sizeof(struct ximg_header);

    if(memcmp(image->header.fileid, "Ximg\r\n10", 8) != 0){
        free(image);
        return 0;
    }

    if(image->header.size > size){
        free(image);
        return 0;
    }

    image->chunks = malloc(sizeof(struct xchu*) * (image->header.chunks));
    if(!image->chunks){
        free(image);
        return 0;
    }

    int index = 0;
    while(index < image->header.chunks){
        uint32_t chunk_size = *(uint32_t*)source;

        struct xchu * chunk = (struct xchu*)malloc(chunk_size);
        memcpy(chunk, source, chunk_size);
        source+= chunk_size;

        image->chunks[index] = chunk;
        index++;
    }

    return image;

}

int ximg_dump(struct ximg * image, void * destination, uint32_t size){
    if(!image) return 0;
    if(size < image->header.size) return 0;

    memcpy(destination, &image->header, sizeof(struct ximg_header));
    destination+= sizeof(struct ximg_header);

    int index = 0;
    while(index < image->header.chunks){
        memcpy(destination, image->chunks[index], image->chunks[index]->size);
        destination+= image->chunks[index]->size;
        index++;
    }

    return 1;
}