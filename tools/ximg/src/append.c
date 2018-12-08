#include <stdio.h>

int help_append(int ret){
	puts("Usage: ximg append <target> <source> [options]");
	puts(" options:");
	return ret;
}

int action_append(int argc, const char ** argv){
	return help_append(3);
}