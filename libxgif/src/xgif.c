#include <ximg/xgif.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ximg/xmap.h>
#include <ximg/xbitmap.h>
#include <ximg/xtypes.h>
#include <compression/lzw.h>

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

struct xgif_fragment {
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
	unsigned char minimumCodeSize;
} __attribute__((__packed__));

struct xgif_extension {
	unsigned char type;
	unsigned char size;
} __attribute__((__packed__));

static inline unsigned int xgif_frame_count(FILE * f){
	return 1;
}

static inline int xgif_read_block(FILE * f, void * buffer){
	unsigned char length;
	fread(&length, 1, 1, f);

	if(length > 0){
		if(fread(buffer, 1, length, f) != length) return -1;
	}

	return length;
}

static inline unsigned int xgif_read_palette(struct ximg * image, FILE * f, int size){
	unsigned int id = xpal_create(image, XPAL_RGB8, size);
	if(!id) return 0;

	struct xpal * palette = xpal_get_by_id(image, id);
	if(!palette){
		printf("Failed to get palete just created");
		return 0;
	}

	struct xpixel color;
	for(int i = 0; i < size; i++){
		if(!fread(&color, 3, 1, f)) return 0;
		xpal_set_rgb(palette, i, &color);
	}
	return id;
}

struct ximg * xgif_load(const char * filename){
	FILE * f = fopen(filename, "rb");
	if(!f) {
		printf("File %s not found", filename);
		return 0;
	}

	char identifier[6];
	memset(&identifier, 0, 6);
	fread(&identifier, 1, 6, f);
	if(memcmp(&identifier, "GIF89a", 6) != 0){
		printf("File not supported");
		fclose(f);
		return 0;
	}

	struct xgif_header header;
	if(!fread(&header, sizeof(struct xgif_header), 1, f)){
		printf("Failed to read header");
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

    struct ximg * image = ximg_create();
	if(!image) {
		printf("Failed to create ximg");
		return 0;
	}

	unsigned int globalPalette = 0;

	if(header.hasTable){
		globalPalette = xgif_read_palette(image, f, 1 << (header.tableSize + 1));
		if(!globalPalette){
			printf("Failed to read palette");
			ximg_free(image);
			fclose(f);
			return 0;
		}
	}

	unsigned int frames = xgif_frame_count(f);
	if(frames == 0){
		printf("No image data found");
		ximg_free(image);
		fclose(f);
		return 0;
	}

	if(frames > 1){
		printf("Animated GIF's not yet supported");
		ximg_free(image);
		fclose(f);
		return 0;
	}

	char indicator;
	if(!fread(&indicator, 1, 1, f)){
		printf("Failed to read image");
		fclose(f);
		return 0;
	}

	if(indicator != ',') {
		printf("indicator mis match");
		fclose(f);
		return 0;
	}

	struct xgif_fragment fragment;
	if(!fread(&fragment, sizeof(struct xgif_fragment), 1, f)){
		printf("Failed to read image");
		fclose(f);
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

	unsigned int palette;
	if(fragment.hasTable){
		palette = xgif_read_palette(image, f, 1 << (fragment.tableSize + 1));
		if(!palette){
			printf("Failed to read palette");
			ximg_free(image);
			fclose(f);
			return 0;
		}
	}else{
		palette = globalPalette;
	}

	unsigned int mapped = xmap_create_with_palette(image, header.width, header.height, palette);
	if(!mapped){
		printf("Failed to craate mapped");
		ximg_free(image);
		fclose(f);
		return 0;
	}

	struct xchan * channel = xmap_channel(image, xmap_get_by_id(image, mapped));

	struct lzw_info info;
	info.count  = 1 << fragment.minimumCodeSize;
	info.clear  = info.count;
	info.stop   = info.clear + 1;
	lzw_init(&info);

	int codes = 0;

	int length;
	unsigned char buffer[255];
	while((length = xgif_read_block(f, &buffer)) > 0){
		if(length < 0){
			printf("Failed to read block");
			ximg_free(image);
			fclose(f);
			return 0;
		}

		int decoded;
		for(int i = 0; i < length; i++){
			if(lzw_decode(&info, buffer[i], &decoded) < 0){
				printf("Failed to decode stream");
				ximg_free(image);
				fclose(f);
				return 0;
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
	
	printf("\n-------------------\n");
	printf("codes: %d\n", codes);
	printf("-------------------\n");

	lzw_free(&info);
	fclose(f);

   	return image;
}

int xgif_save(struct ximg * image, ximgid_t id, const char * filename){

}