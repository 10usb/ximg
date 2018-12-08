#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ximg/xtranslate.h>
#include <ximg/xpal.h>

int help_translate(int ret){
	puts("Usage: ximg translate <input> <id> [options]");
	puts(" options:");
	//puts("  --all           - show all info");

	return ret;
}

int action_translate(int argc, const char ** argv){
	if(argc < 4) return help_translate(3);

	const char * input = argv[2];

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