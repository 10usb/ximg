#include <ximg/xgif.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ximg/xmap.h>
#include <ximg/xtypes.h>
#include <compression/lzw.h>
#include "gif.h"

static inline long xgif_global_palette_size(struct xgif_header * header){
	if(header->hasTable) return (1 << (header->tableSize + 1)) * 3;
	return 0;
}
static inline long xgif_local_palette_size(struct xgif_fragment * fragment){
	if(fragment->hasTable) return (1 << (fragment->tableSize + 1)) * 3;
	return 0;
}

static inline long xgif_data_start(struct xgif_header * header){
	return 6 + sizeof(struct xgif_header) + xgif_global_palette_size(header);
}

static inline int xgif_skip_blockchain(FILE * f){
	uint8_t length;
	do {
		if(!fread(&length, 1, 1, f)) return 0;

		if(length > 0){
			if(fseek(f, length, SEEK_CUR) != 0) return 0;
		}
	} while(length > 0);

	return 1;
}

static inline int xgif_read_block(FILE * f, void * buffer){
	uint8_t length;
	fread(&length, 1, 1, f);

	if(length > 0){
		if(fread(buffer, 1, length, f) != length) return -1;
	}

	return length;
}

static inline long xgif_frame_count(struct xgif_header * header, FILE * f){
	long original = ftell(f);
	long count = 0;
	int delayed = 1; // first image is always delayed

	fseek(f, xgif_data_start(header), SEEK_SET);
	uint8_t indicator;

	if(!fread(&indicator, 1, 1, f)) goto end;
	
	do {
		if(indicator == ',') {
			if(delayed){
				delayed = 0;
				count++;
			}

			struct xgif_fragment fragment;
			if(!fread(&fragment, sizeof(struct xgif_fragment), 1, f)){
				printf("Failed to read fragment header\n");
				count = 0;
				goto end;
			}

			long palette = xgif_local_palette_size(&fragment);
			if(palette > 0) fseek(f, palette, SEEK_CUR);
			
			xgif_skip_blockchain(f);
		}else if(indicator == '!') {
			uint8_t code;
			if(!fread(&code, 1, 1, f)){
				printf("Failed to read code\n");
				count = 0;
				goto end;
			}
			if(code == 0xF9){
				union {
					struct xgif_graphic_control control;
					unsigned char dummy[255];
				} buffer;

				if(xgif_read_block(f, &buffer) < sizeof(struct xgif_graphic_control)){
					if(buffer.control.delay > 0) delayed = 1;
				}
			}
			xgif_skip_blockchain(f);
		}else {
			printf("Unexpected indicator 0x%X @ %X\n", indicator, ftell(f) - 1);
			count = 0;
			goto end;
		}

		if(!fread(&indicator, 1, 1, f)){
			printf("Failed to read next indicator\n");
			count = 0;
			goto end;
		}
	}while(indicator != ';');

	end:
	fseek(f, original, SEEK_SET);
	return count;
}

static inline ximgid_t xgif_read_palette(struct ximg * image, FILE * f, int size){
	ximgid_t id = xpal_create(image, XPAL_RGB8, size);
	if(!id) return 0;

	struct xpal * palette = xpal_get_by_id(image, id);
	if(!palette) return 0;

	struct xpixel color;
	for(int i = 0; i < size; i++){
		if(!fread(&color, 3, 1, f)) return 0;
		xpal_set_rgb(palette, i, &color);
	}
	return id;
}

static inline int xgif_read_fragment(struct ximg * image, FILE * f, struct xgif_state * state){
	struct xgif_fragment fragment;
	if(!fread(&fragment, sizeof(struct xgif_fragment), 1, f)){
		printf("Failed to read image\n");
		return 0;
	}

	printf("left           : %d\n", fragment.left);
	printf("top            : %d\n", fragment.top);
	printf("width          : %d\n", fragment.width);
	printf("height         : %d\n", fragment.height);
	printf("hasTable       : %d\n", fragment.hasTable);
	printf("interlaced     : %d\n", fragment.interlaced);
	printf("sortFlag       : %d\n", fragment.sortFlag);
	printf("tableSize      : %d\n", 1 << (fragment.tableSize + 1));
	printf("reserved       : %d\n", fragment.reserved);
	printf("minimumCodeSize: %d\n", fragment.minimumCodeSize);
	printf("-------------------\n");

	if(fragment.hasTable){
		state->palettes.active = xgif_read_palette(image, f, 1 << (fragment.tableSize + 1));
		if(!state->palettes.active){
			printf("Failed to read palette\n");
			return -1;
		}
	}else if(state->palettes.global){
		state->palettes.active = state->palettes.global;
	}else if(state->palettes.initial){
		state->palettes.active = state->palettes.initial;
	}

	ximgid_t mapped = xmap_create_with_palette(image, fragment.width, fragment.height, state->palettes.active, state->control.transparent);
	if(!mapped){
		printf("Failed to craate mapped\n");
		return -1;
	}

	if(state->control.transparent){
		xmap_set_transparent(image, mapped, state->control.transparentIndex);
	}

	struct xchan * channel = xmap_channel(image, xmap_get_by_id(image, mapped));

	struct lzw_info info;
	info.count  = 1 << fragment.minimumCodeSize;
	info.clear  = info.count;
	info.stop   = info.clear + 1;
	lzw_init(&info);

	int codes = 0;

	int length;
	uint8_t buffer[255];
	while((length = xgif_read_block(f, &buffer)) > 0){
		if(length < 0){
			printf("Failed to read block\n");
			return -1;
		}

		int decoded;
		for(int i = 0; i < length; i++){
			if(lzw_decode(&info, buffer[i], &decoded) < 0){
				printf("Failed to decode stream\n");
				return -1;
			}

			if(decoded > 0){
				for(int i = 0; i < decoded; i++){
					int code = info.codes[i];

					for(int j = 0; j < info.entries[code].length; j++){
						int x = codes % fragment.width, y = codes / fragment.width;
						xchan_set8(channel, fragment.left + x, y, info.entries[code].value[j]);
						codes++;
					}
				}
			}
		}
	}
	
	// printf("\n-------------------\n");
	// printf("codes: %d\n", codes);
	// printf("-------------------\n");

	lzw_free(&info);
}

static inline int xgif_read_extension(struct ximg * image, FILE * f, struct xgif_state * state){
	uint8_t code;
	if(!fread(&code, 1, 1, f)) return 0;

	if(code == 0xF9){
		 if(!xgif_skip_blockchain(f)) return 0;
	}else{
		if(!xgif_skip_blockchain(f)) return 0;
	}
	return 1;
}

struct ximg * xgif_load(const char * filename){
	FILE * f = fopen(filename, "rb");
	if(!f) return 0;

	char identifier[6];
	memset(&identifier, 0, 6);
	fread(&identifier, 1, 6, f);
	if(memcmp(&identifier, "GIF89a", 6) != 0){
		fclose(f);
		return 0;
	}

	struct xgif_header header;
	if(!fread(&header, sizeof(struct xgif_header), 1, f)){
		fclose(f);
		return 0;
	}
	
	printf("width          : %d\n", header.width);
	printf("height         : %d\n", header.height);
	printf("hasTable       : %d\n", header.hasTable);
	printf("colorResolution: %d\n", header.colorResolution);
	printf("sortFlag       : %d\n", header.sortFlag);
	printf("tableSize      : %d\n", 1 << (header.tableSize + 1));
	printf("backgroundIndex: %d\n", header.backgroundIndex);
	printf("pixelRatio     : %d\n", header.pixelRatio);
	printf("-------------------\n");

	if(!header.hasTable){
		// detect corrupt header, and try to fix it
	}

    struct ximg * image = ximg_create();
	if(!image) {
		fclose(f);
		return 0;
	}

	struct xgif_state state;
	memset(&state, 0, sizeof(struct xgif_state));

	if(header.hasTable){
		state.palettes.global = xgif_read_palette(image, f, 1 << (header.tableSize + 1));
		if(!state.palettes.global){
			ximg_free(image);
			fclose(f);
			return 0;
		}
	}

	long frames = xgif_frame_count(&header, f);
	printf("frames     : %d\n", frames);

	if(frames == 0){
		ximg_free(image);
		fclose(f);
		return 0;
	}

	// if(frames > 1){
	// 	printf("Animated GIF's not yet supported\n");
	// 	ximg_free(image);
	// 	fclose(f);
	// 	return 0;
	// }

	char indicator;
	if(!fread(&indicator, 1, 1, f)){
		ximg_free(image);
		fclose(f);
		return 0;
	}
	
	int frame = 0;
	do {
		if(indicator == ',') {
			printf("frame          : %d\n", frame++);
			if(xgif_read_fragment(image, f, &state) < 0){
				ximg_free(image);
				fclose(f);
				return 0;
			}
		}else if(indicator == '!') {
			if(xgif_read_extension(image, f, &state) < 0){
				ximg_free(image);
				fclose(f);
				return 0;
			}
		}else{
			printf("indicator mis match\n");
			ximg_free(image);
			fclose(f);
			return 0;
		}

		if(!fread(&indicator, 1, 1, f)){
			ximg_free(image);
			fclose(f);
			return 0;
		}
	}while(indicator != ';');

	fclose(f);

   	return image;
}