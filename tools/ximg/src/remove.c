#include <stdio.h>

int help_remove(int ret){
	puts("Usage: ximg remove <target> <id> [options]");
	puts(" options:");
	return ret;
}

int action_remove(int argc, const char ** argv){
    return help_remove(3);
}