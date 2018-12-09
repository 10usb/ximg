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
	if(info->next >= 4096) return info->next;

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
	// Make it optional then next could be -1 for more compact encoding
	register int bits = 1, x = info->next;
	while (x >>= 1) bits++;
	return bits;
}

int lzw_decode(struct lzw_info * info, uint8_t data, int * count){
	// offset + length in bits should never be anything else as a multiple of 8
	// thus deviding it by 8 will give an index to a free byte in the buffer
	int index = (info->offset + info->length) / 8;
	
	// max values of 7 bits offset + 12 bits length should never be more then 3 bytes
	if(index > 3){
		return -1;
	}

	// Store the new data in the buffer
	info->buffer[index] = data;
	info->length+= 8;

	// initialize count with 0 codes decodes
	*count = 0;
	
	// While the new data in the buffer is larger then the amount of bits the table need
	// we can read a code
	int bits;
	while(info->length >= (bits = lzw_bits(info))){
		unsigned int code;

		// Ooh fuck it, just assume little endian, big endian will be done later
		code = *(int*)(&info->buffer);
		code = code >> info->offset;

		unsigned int mask = ((1 << bits) - 1);
		code = code & mask;

		if(code == info->stop){
			return 0;
		}

		if(code == info->clear){
			lzw_clear(info);
		}else{
			// Only with the first code this needs not to be done, we can check with the existance of a previous read code
			if(info->lastCode >= 0){
				if(code < info->next){
					// When the code already exist we'll use the last code + the firts byte of this code
					// to create a new code, it is doesn't already exist
					unsigned char next = info->entries[code].value[0];
					if(!lzw_lookup(info, info->lastCode, next)){
						lzw_add(info, next);
					}
				}else if(code == info->next){
                    // Whe the code is equal to new next code to create we'll use the last code + the first byte
                    // of the last code to create the next code
					lzw_add(info, info->entries[info->lastCode].value[0]);
				}else{
					// This should never happen, as this means we've read a code value we don't know the value for yet
					return -1;
				}
			}

            // Store the code in the array of decode codes
			info->codes[*count] = code;
			*count = (*count) + 1;

			info->lastCode = code;
		}

        // While the offset is larger then one byte shift the bytes in the buffer
		info->length-= bits;
		info->offset+= bits;
		while(info->offset >= 8){
			info->buffer[0] = info->buffer[1];
			info->buffer[1] = info->buffer[2];
			info->buffer[2] = 0;
			info->offset-= 8;
		}
	}

	return 1;
}