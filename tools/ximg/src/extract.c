#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/ximg.h>
#include <ximg/xwriter.h>

int help_extract(int ret){
    puts("Usage: ximg extract <input> <output> [options]");
    puts(" options:");
    puts("  --format=<n>    - the format target will be in");
    puts("  --help          - shows help about the action");
    puts("  --index=<n>     - index of the type image to copy/extract");
    puts("  --type=<n>      - type of the instance to perform the acion on (default:xras)");
    return ret;
}

int action_extract(int argc, const char ** argv){
    if(argc < 4) return help_extract(-3);

    unsigned short index = 0;
    const char * input = argv[2];
    const char * output = argv[3];
    unsigned int type = XRAS_RGBA;

    for(int i = 4; i < argc; i++){
        if(strcmp(argv[i], "--help") == 0){
            return help_extract(0);
        }else if(strncmp(argv[i], "--index=", sizeof("--index")) == 0){
            if(sscanf(argv[i] + sizeof("--index"),"%hu", &index) <= 0){
                fprintf(stderr, "Argument of index is not a number\n");
                return -5;
            }
        }else{
            fprintf(stderr, "Invalid argument %s\n", argv[i]);
            return -4;
        }
    }

    struct ximg * source = ximg_load(input);
    if(!source) return -6;

    struct xreader reader;
    if(!xreader_init(&reader, source, index)){
        ximg_free(source);
        return -6;
    }

    struct ximg * destination = ximg_create();

    struct xwriter writer;
    if(!xwriter_create(&writer, destination, reader.width, reader.height, type)){
        xreader_clear(&reader);
        ximg_free(source);
        return -7;
    }

    if(!xwriter_copy(&writer, &reader)){
        xreader_clear(&reader);
        ximg_free(source);
        return -8;
    }

    if(!ximg_save(destination, output)){
        fprintf(stderr, "Failed to save %s\n", output);
        xreader_clear(&reader);
        ximg_free(source);
        ximg_free(destination);
        return -9;
    }

    xreader_clear(&reader);
    ximg_free(source);
    ximg_free(destination);
    return 0;
}