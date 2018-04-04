#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/xgif.h>
#include <ximg/xtranslate.h>

int main(int argc, const char ** argv){
  if(argc < 2){
    printf("No file given");
    return 1;
  }
  
  const char * file = argv[1];

  struct ximg * image = xgif_load(file);
  if(image){
    ximg_save(image, "bin/output.ximg");

    struct ximg * dest = ximg_create();

    xtranslate_mapped(image, 0, dest);

    ximg_save(dest, "bin/output.raster.ximg");
  }
}