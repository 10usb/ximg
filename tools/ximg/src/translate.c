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

    fprintf(stderr, "Unsupported type to translate");
    return 4;
}

int old_translate(){
	struct ximg * image = 0;
	if(image){
		ximg_save(image, "bin/output.ximg");

		struct ximg * raster = ximg_create();

		xtranslate_mapped(image, 0, raster);

		ximg_save(raster, "bin/output.raster.ximg");
	}

	{
		struct ximg * raster = ximg_load("bin/source.raster.ximg");

		struct ximg * mapped = ximg_create();

		struct xpal * palette = xpal_get_by_id(mapped, xpal_create(mapped, XPAL_RGB8, 27));
		
		struct xpixel pixel;

		for(int r = 0; r < 3; r++){
			for(int g = 0; g < 3; g++){
				for(int b = 0; b < 3; b++){
					pixel.r = (int)(127.5 * r);
					pixel.g = (int)(127.5 * g);
					pixel.b = (int)(127.5 * b);
					xpal_set_rgb(palette,  r * 9 + g * 3 + b, &pixel);
				}
			}
		}

		xtranslate_raster_with_palette(raster, 0, mapped, 0);

		ximg_save(mapped, "bin/output.mapped.ximg");
	}
}