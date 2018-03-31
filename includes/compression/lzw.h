#ifndef LZW_H
#define LZW_H

#include <stdint.h>

struct lzw_entry {
  uint8_t * value;
  int length;
};

struct lzw_info {
  int count;
  int clear;
  int stop;
  int next;
  int lastCode;
  int offset;
  int length;
  unsigned char buffer[3];
  int codes[4];
  struct lzw_entry entries[4096];
};

void lzw_init(struct lzw_info * info);
void lzw_clear(struct lzw_info * info);
void lzw_free(struct lzw_info * info);
int lzw_decode(struct lzw_info * info, uint8_t data, int * count);
int lzw_get_code(struct lzw_info * info, int code, void * buffer);

#endif
