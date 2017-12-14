#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/ximg.h>
#include <ximg/xras.h>

int help(int ret){
    puts("Usage: ximg <action> <parameters>");
    puts(" actions:");
    puts("  append <output> <input> [options]");
    puts("  extract <input> <output> [options]");
    puts("  remove <input> [options]");
    puts(" options:");
    puts("  --format=<n>    - the format target will be in");
    puts("  --help          - show this help");
    puts("  --index=<n>     - index of the type image to copy/extract");
    puts("  --type=<n>      - type of the instance to perform the acion on (default:xras)");
    return ret;
}

int main(int argc, const char ** argv){
    if(argc < 2) return help(-1);

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--help") == 0){
            return help(0);
        }else{
            fprintf(stderr, "Invalid argument %s\n", argv[i]);
            return -2;
        }
    }
    
    return 0;
}
