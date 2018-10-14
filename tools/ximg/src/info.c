#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/ximg.h>

int help_info(int ret){
    puts("Usage: ximg info <input> [options]");
    puts(" options:");
    puts("  --all           - show all info");

    return ret;
}

int action_info(int argc, const char ** argv){
    if(argc < 3) return help_info(3);

    const char * input = argv[2];

    
    struct ximg * image = ximg_load(input);
    if(!image) return 4;
    
    printf("-- Header --\n");
    if(image->header.size > 1048576){
        printf("size    : %4.2f MB\n", image->header.size / 1048576.0);
    }else if(image->header.size > 1024){
        printf("size    : %4.2f kB\n", image->header.size / 1024.0);
    }else{
        printf("size    : %d bytes\n", image->header.size);
    }
    printf("checksum: %08x\n", image->header.checksum);
    printf("chunks  : %d\n", image->header.chunks);
    printf("flags   : %d\n", image->header.flags);

    printf("\n-- Chunks --\n");
    printf("size       type  id         checksum\n");
    for(int index = 0; index < image->header.chunks; index++){
        uint32_t size = image->chunks[index]->size;
        if(size > 1048576){
            printf("%4.2f MB  ", size / 1048576.0);
        }else if(image->header.size > 1024){
            printf("%4.2f kB  ", size / 1024.0);
        }else{
            printf("%3d bytes  ", size);
        }
        //printf("%011d  ", image->chunks[index]->size);
        printf("%.*s  ", 4, &image->chunks[index]->type);
        printf("%04X-%04X  ", image->chunks[index]->id >> 16, image->chunks[index]->id);
        printf("%08X\n", image->chunks[index]->checksum);
    }

    ximg_free(image);
    return 0;
}