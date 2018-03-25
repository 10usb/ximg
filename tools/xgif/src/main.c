#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/xbitmap.h>
#include <ximg/xtypes.h>
#include <ximg/xgif.h>
#include <compression/lzw.h>

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

    printf("-------------------\n");

    


    struct lzw_info info;
    info.count  = 256;
    info.clear  = 256;
    info.stop   = 257;
    lzw_init(&info);

    unsigned char c;
    int decoded;

    fread(&c, 1, 1, f); // length?

    fread(&c, 1, 1, f);
    // printf("{%02X}", c);

    unsigned char data[120 + 50];

    int limit = 80;
    int codes = 0;
    while(limit-- && lzw_decode(&info, c, &decoded) > 0){
      if(decoded > 0){
        // printf(" => ");
        for(int i = 0; i < decoded; i++){
          // printf("[%2d]", info.codes[i]);
          int code = info.codes[i];

          for(int j = 0; j < info.entries[code].length; j++){
            data[codes++] = info.entries[code].value[j];
          }
        }
        // printf("\n");
      }

      if(!fread(&c, 1, 1, f)){
        printf("Failed to read data");
        lzw_free(&info);
        fclose(f);
        return 3;
      }
      // printf("{%02X}", c);
    }
    
    printf("\n-------------------\n");
    printf("codes: %d\n", codes);

    printf("-------------------\n");
    for(int y = 0; y < 10; y++){
      for(int x = 0; x < 12; x++){
          printf("%d ", data[y * 12 + x]);
      }
      printf("\n");
    }
    printf("-------------------\n");
    // for(int code = 256; code < info.next; code++){
    //   int addres = (int)info.entries[code].value;
    //   addres-= (int)info.entries[256].value;
    //   printf("%3d => %3d: ", code, addres);

    //   for(int i = 0; i < info.entries[code].length; i++){
    //       printf("%d ", info.entries[code].value[i]);
    //   }

    //   printf("(%d)\n", info.entries[code].length);
    // }
    // printf("-------------------\n");

    lzw_free(&info);
    fclose(f);
}