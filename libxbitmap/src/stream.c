#include "bitmap.h"


void bitmap_stream_init(struct bitmap_stream * stream, uint32_t offset, int bits, uint32_t width){
    stream->offset = offset;
    stream->bits = bits;
    stream->scanline = (bits * width + 31 - (bits * width - 1) % 32) / 8;
    stream->position = 0;
    stream->buffer = 0;
    stream->mask = (1 << bits)  - 1;
}

void bitmap_stream_align_read(struct bitmap_stream * stream, FILE * f){
    long position = ftell(f);
    long remain = (position - stream->offset) % stream->scanline;
    if(remain > 0) fseek(f, stream->scanline - remain, SEEK_CUR);
    stream->position = 0;
}

int bitmap_stream_read(struct bitmap_stream * stream, FILE * f, uint8_t * result){
    if(stream->bits == 8){
        return fread(result, sizeof(uint8_t), 1, f);
    }

    uint8_t value;

    stream->position-= stream->bits;

    if(stream->position < 0){
        value = stream->buffer << (0 - stream->position);

        if(!fread(&stream->buffer, sizeof(uint8_t), 1, f)) return 0;
        
        uint8_t mask = (1 << (0 - stream->position))  - 1;
        value|= (stream->buffer >> (stream->position + 8)) & mask;

        stream->position+= 8;
    }else{
        value = stream->buffer >> stream->position;
    }
    
    *result = value & stream->mask;

    return 1;
}

int bitmap_stream_align_write(struct bitmap_stream * stream, FILE * f){
    if(stream->position > 0){
        if(!fwrite(&stream->buffer, sizeof(uint8_t), 1, f)) return 0;
    }
    long position = ftell(f);
    long remain = (position - stream->offset) % stream->scanline;
    if(remain > 0){
        remain = stream->scanline - remain;
        
        while(remain > 4){
            if(!fwrite("\0\0\0\0", 4, 1, f)) return 0;
            remain-=4;
        }
        if(!fwrite("\0\0\0\0", remain, 1, f)) return 0;
    }

    stream->position = 0;
    stream->buffer = 0;
}

int bitmap_stream_write(struct bitmap_stream * stream, FILE * f, uint8_t value){
    if(stream->bits == 8){
        return fwrite(&value, sizeof(uint8_t), 1, f);
    }

    stream->position+= stream->bits;
    if(stream->position < 8){
        stream->buffer|= (value & stream->mask) << (8 - stream->position);
    }else{
        stream->position-= 8;
        stream->buffer|= (value & stream->mask) >> stream->position;
        
        if(!fwrite(&stream->buffer, sizeof(uint8_t), 1, f)) return 0;
        stream->buffer= value << (8 - stream->position);
    }
}