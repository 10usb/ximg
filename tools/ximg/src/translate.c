#include "main.h"
#include <ximg/xtranslate.h>
#include <ximg/xpal.h>

int help_translate(int ret){
	puts("Usage: ximg translate <input> <id> [options]");
	puts(" options:");
	puts("  --output=<file>  - the file to output the result (default <input>.<type>.ximg)");

	return ret;
}

int action_translate_mapped(struct ximg * image, ximgid_t id, int argc, const char ** argv){
    const char * output = 0;
    char * generated = 0;

    for(int i = 4; i < argc; i++){
        if(strncmp(argv[i], "--output=", sizeof("--output")) == 0){
            output = argv[i] + sizeof("--output");
        }else{
            fprintf(stderr, "Invalid argument %s\n", argv[i]);
            ximg_free(image);
            return 5;
        }
    }

    if(!output){
        output = generated = makename(argv[2], ".raster.ximg");
    }

    struct ximg * raster = ximg_create();

    if(!xtranslate_mapped(image, id, raster)){
        if(generated) free(generated);
        ximg_free(raster);
        ximg_free(image);
        return 6;
    }

    ximg_save(raster, output);

    if(generated) free(generated);
    ximg_free(raster);
    ximg_free(image);
    return 0;
}

int action_translate_raster(struct ximg * image, ximgid_t id, int argc, const char ** argv){
    const char * output = 0;
    char * generated = 0;

    for(int i = 4; i < argc; i++){
        if(strncmp(argv[i], "--output=", sizeof("--output")) == 0){
            output = argv[i] + sizeof("--output");
        }else{
            fprintf(stderr, "Invalid argument %s\n", argv[i]);
            ximg_free(image);
            return 5;
        }
    }

    if(!output){
        output = generated = makename(argv[2], ".mapped.ximg");
    }
    
    struct ximg * mapped = ximg_create();
    ximgid_t palette_id = xpal_create(mapped, XPAL_RGB8, 216);
    struct xpal * palette = xpal_get_by_id(mapped, palette_id);
    
    struct xpixel pixel;
    for(int r = 0; r < 6; r++){
        for(int g = 0; g < 6; g++){
            for(int b = 0; b < 6; b++){
                pixel.r = (int)(255.0 * r / 5);
                pixel.g = (int)(255.0 * g / 5);
                pixel.b = (int)(255.0 * b / 5);
                xpal_set_rgb(palette,  r * 36 + g * 6 + b, &pixel);
            }
        }
    }

    if(!xtranslate_raster_with_palette(image, id, mapped, palette_id)){
        if(generated) free(generated);
        ximg_free(mapped);
        ximg_free(image);
        return 6;
    }

    ximg_save(mapped, output);

    if(generated) free(generated);
    ximg_free(mapped);
    ximg_free(image);
    return 0;
}

int action_translate(int argc, const char ** argv){
	if(argc < 4) return help_translate(3);


    struct ximg * image = ximg_load(argv[2]);
    ximgid_t id = atoll(argv[3]);

    struct xchu * chunk = ximg_get(image, id);

    if(!chunk){
        fprintf(stderr, "Id %04X-%04X not found", id >> 16, id);
        return 3;
    }

    if(chunk->type == ximg_make("XMAP")){
        return action_translate_mapped(image, id, argc, argv);
    }

    if(chunk->type == ximg_make("XRAS")){
        return action_translate_raster(image, id, argc, argv);
    }

    fprintf(stderr, "Unsupported type to translate");
    return 4;
}