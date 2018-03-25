#include <compression/lzw.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void lzw_init(struct lzw_info * info){
  lzw_clear(info);

  info->offset    = 0;
  info->length    = 0;

  // All dictionary values together can become 8.1MB
  unsigned char * buffer = malloc(9 * 1024 * 1024);
  int count = 0;
  for(int index = 0; count < info->count; index++){
    if(index != info->clear && index != info->stop){
      buffer[count] = count;
      info->entries[index].value = buffer + count;
      info->entries[index].length = 1;
      count++;
    }
  }

  if(info->clear >= 0){
      info->entries[info->clear].value  = buffer + count;
      info->entries[info->clear].length = 0;
  }
  if(info->stop >= 0){
      info->entries[info->stop].value  = buffer + count;
      info->entries[info->stop].length = 0;
  }
}

void lzw_free(struct lzw_info * info){
	free(info->entries[0].value);
}

inline void lzw_clear(struct lzw_info * info) {
  info->lastCode  = -1;
  info->next      = info->count;
  if(info->clear >= 0) info->next++;
  if(info->stop >= 0) info->next++;
}

static inline struct lzw_entry * lzw_lookup(struct lzw_info * info, int index, unsigned char next){
  struct lzw_entry * entry = &info->entries[index++];

  while(index < info->next){
    if(info->entries[index].length == (entry->length + 1)
        && memcmp(info->entries[index].value, entry->value, entry->length) == 0
        && info->entries[index].value[entry->length] == next
      ) return &info->entries[index];
    index++;
  }

  return 0;
}

static inline unsigned char * lzw_buffer(struct lzw_info * info){
  struct lzw_entry * lastEntry = &info->entries[info->next - 1];
  return lastEntry->value + lastEntry->length;
}

static inline int lzw_add(struct lzw_info * info, unsigned char next){
  struct lzw_entry * lastCode = &info->entries[info->lastCode];
  unsigned char * buffer = lzw_buffer(info);  
  memcpy(buffer, lastCode->value, lastCode->length);
  buffer[lastCode->length] = next;

  struct lzw_entry * nextEntry = &info->entries[info->next];
  nextEntry->value = buffer;
  nextEntry->length = lastCode->length + 1;
  
  return info->next++;
}

int lzw_get_code(struct lzw_info * info, int code, void * buffer){
  if(buffer){
    struct lzw_entry * entry = &info->entries[code];
    memcpy(buffer, entry->value, entry->length);
  }
  return info->entries->length;
}

static inline int lzw_bits(struct lzw_info * info){
  register int bits = 1, x = info->next - 1;
  while (x >>= 1) bits++;
  return bits;
}

int lzw_decode(struct lzw_info * info, unsigned char data, int * count){
  // Store the new data and the end of the buffer
  info->buffer[(info->offset + info->length) / 8] = data;

  // Move length by 8
  info->length+= 8;

  *count = 0;
  
  int bits;
  while(info->length >= (bits = lzw_bits(info))){
    int code;

    // Ooh fuck it, just assume little endian, big endian will be done later
    int mask = ((1 << bits) - 1);
    code = *(int*)(&info->buffer);
    code = code >> info->offset;
    code = code & mask;

    if(code == info->stop) return 0;

    if(code == info->clear){
      lzw_clear(info);
    }else{

      if(info->lastCode >= 0){
        if(code < info->next){
          unsigned char next = info->entries[code].value[0];
          if(!lzw_lookup(info, info->lastCode, next)){
            lzw_add(info, next);
          }
        }else if(code == info->next){
          lzw_add(info, info->entries[info->lastCode].value[0]);
        }else{
          printf("Invalid code %d >= %d\n", code, info->next);
          return -1;
        }
      }

      info->codes[*count] = code;
      *count = (*count) + 1;

      info->lastCode = code;
    }

    info->length-= bits;
    info->offset+= bits;
    while(info->offset >= 8){
      info->buffer[0] = info->buffer[1];
      info->buffer[1] = info->buffer[2];
      info->buffer[2] = info->buffer[3];
      info->offset-= 8;
    }
  }

	return 1;
}