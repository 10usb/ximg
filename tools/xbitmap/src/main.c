#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/xbitmap.h>

int help(int ret){
    puts("Usage: xbitmap [options] file");
    puts(" options:");
    puts("  --bits=<n>      - the number of bits per pixel to use fo the bitmap");
    puts("  --help          - show this help");
    puts("  --index=<n>     - index of the raster image to extract");
    puts("  --output=<file> - the target to create the new file");
    return ret;
}

char * makeoutput(const char * input, const char * extension){
    int length = strlen(input);
    while(length > 0 && input[length] != '.') length--;
    
    if(!length) {
        length = strlen(input);
    }

    char * output = malloc(length + strlen(extension) + 1);
    memcpy(output, input, length);
    memcpy(output + length, extension, strlen(extension) + 1);
    return output;
}

int main(int argc, const char ** argv){
    unsigned short index = 0;
    unsigned char bits = 24;
    const char * input = 0;
    const char * output = 0;
    struct ximg * image;

    if(argc < 2) return help(-4);

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--help") == 0){
            return help(0);
        }else if(strncmp(argv[i], "--index=", sizeof("--index")) == 0){
            if(sscanf(argv[i] + sizeof("--index"),"%hu", &index) <= 0){
                fprintf(stderr, "Argument of index is not a number\n");
                return -1;
            }
        }else if(strncmp(argv[i], "--bits=", sizeof("--bits")) == 0){
            if(sscanf(argv[i] + sizeof("--bits"),"%hhu", &bits) <= 0){
                fprintf(stderr, "Argument of bits is not a number\n");
                return -2;
            }
        }else if(strncmp(argv[i], "--output=", sizeof("--output")) == 0){
            output = argv[i] + sizeof("--output");
        }else if((i+1) == argc){
            input = argv[i];
        }else{
            fprintf(stderr, "Invalid argument %s\n", argv[i]);
            return -3;
        }
    }

    if(image = xbitmap_load(input)){
        if(!output) output = makeoutput(input, ".ximg");

        if(!ximg_save(image, output)){
            fprintf(stderr, "Failed to save %s\n", output);
            ximg_free(image);
            return -5;
        }
        ximg_free(image);
    }else if(image = ximg_load(input)){
        if(!output) output = makeoutput(input, ".bmp");

        if(!xbitmap_save(image, index, output, bits)){
            fprintf(stderr, "Failed to save %s\n", output);
            ximg_free(image);
            return -6;
        }
        
        ximg_free(image);
    }else{
        fprintf(stderr, "Failed to open %s\n", input);
        ximg_free(image);
        return -7;
    }
    
    return 0;
}
