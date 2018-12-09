#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int action_info(int argc, const char ** argv);
int action_append(int argc, const char ** argv);
int action_extract(int argc, const char ** argv);
int action_remove(int argc, const char ** argv);
int action_translate(int argc, const char ** argv);

char * makename(const char * source, const char * extension);

#endif