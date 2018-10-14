#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int action_info(int argc, const char ** argv);
int action_append(int argc, const char ** argv);
int action_extract(int argc, const char ** argv);
int action_remove(int argc, const char ** argv);
int action_translate(int argc, const char ** argv);

int help(int ret){
    puts("Usage: ximg <action> [parameters]");
    puts(" actions:");
    puts("  info <file> [options]");
    puts("  append <output> <input> [options]");
    puts("  extract <input> <output> [options]");
    puts("  remove <input> [options]");
    puts("  translate <input> <output> [options]");
    puts("  help");
    puts("");
    puts(" for info per action use:");
    puts("  ximg <action> --help");
    return ret;
}

int main(int argc, const char ** argv){
    if(argc < 2) return help(-1);

    if(strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0){
        return help(0);
    }else if(strcmp(argv[1], "info") == 0){
        return action_info(argc, argv);
    }else if(strcmp(argv[1], "append") == 0){
        return action_append(argc, argv);
    }else if(strcmp(argv[1], "extract") == 0){
        return action_extract(argc, argv);
    }else if(strcmp(argv[1], "remove") == 0){
        return action_remove(argc, argv);
    }else{
        fprintf(stderr, "Invalid action '%s'\n", argv[1]);
        return -2;
    }

    return 0;
}
