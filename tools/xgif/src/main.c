#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/xbitmap.h>
#include <ximg/xtypes.h>

struct xgif_header {
  unsigned short width;
  unsigned short height;
  union {
    unsigned char flags;
    struct {
        unsigned char hasTable : 1;
        unsigned char colorResolution : 3;
        unsigned char sortFlag : 1;
        unsigned char tableSize : 3;
    } __attribute__((__packed__));
  };
  unsigned char backgroundIndex;
  unsigned char pixelRatio;
} __attribute__((__packed__));

struct xgif_image {
  unsigned short left;
  unsigned short top;
  unsigned short width;
  unsigned short height;
  union {
    unsigned char flags;
    struct {
        unsigned char hasTable : 1;
        unsigned char interlaced : 1;
        unsigned char sortFlag : 1;
        unsigned char reserved : 2;
        unsigned char tableSize : 3;
    } __attribute__((__packed__));
  };
} __attribute__((__packed__));

struct lzw_entry {
  unsigned char * value;
  int length;
};
struct lzw_info {
  short clear;
  short stop;
  short count;
  struct lzw_entry entries[4096];
  short lastCode;
  unsigned char lastRead[2];
};

int main(int argc, const char ** argv){
    FILE * f = fopen("resources/Naamloos.gif", "rb");
    if(!f) {
      printf("File %s not found", "resources/Naamloos.gif");
      return 1;
    }

    char identifier[6];
    memset(&identifier, 0, 6);
    fread(&identifier, 1, 6, f);
    if(memcmp(&identifier, "GIF89a", 6) != 0){
      printf("File not supported");
      fclose(f);
      return 2;
    }

    struct xgif_header header;
    if(!fread(&header, sizeof(struct xgif_header), 1, f)){
      printf("Failed to read header");
      fclose(f);
      return 3;
    }
    
    printf("width          : %d\n", header.width);
    printf("height         : %d\n", header.height);
    printf("hasTable       : %d\n", header.hasTable);
    printf("colorResolution: %d\n", header.colorResolution);
    printf("sortFlag       : %d\n", header.sortFlag);
    printf("tableSize      : %d\n", 1 << (header.tableSize + 1));
    printf("backgroundIndex: %d\n", header.backgroundIndex);
    printf("pixelRatio     : %d\n", header.pixelRatio);

    struct xpixel palette[256];

    if(header.hasTable){
      int size = 1 << (header.tableSize + 1);
      for(int i = 0; i < size; i++){
        fread(&palette[i], 3, 1, f);
      }
    }

    char indicator;
    if(!fread(&indicator, 1, 1, f)){
      printf("Failed to read image");
      fclose(f);
      return 3;
    }

    if(indicator != ',') {
      printf("indicator mis match");
      fclose(f);
      return 3;
    }

    struct xgif_image image;
    if(!fread(&image, sizeof(struct xgif_image), 1, f)){
      printf("Failed to read image");
      fclose(f);
      return 3;
    }
    printf("left           : %d\n", image.left);
    printf("top            : %d\n", image.top);
    printf("width          : %d\n", image.width);
    printf("height         : %d\n", image.height);
    printf("hasTable       : %d\n", image.hasTable);
    printf("interlaced     : %d\n", image.interlaced);
    printf("sortFlag       : %d\n", image.sortFlag);
    printf("tableSize      : %d\n", 1 << (image.tableSize + 1));
    printf("reserved       : %d\n", image.reserved);

    fclose(f);
}