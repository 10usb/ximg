#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/xgif.h>

int main(int argc, const char ** argv){
  if(argc < 2){
    printf("No file given");
    return 1;
  }
  
  const char * file = argv[1];

  struct ximg * image = xgif_load(file);
  if(image){
    ximg_save(image, "bin/output.ximg");
  }
}