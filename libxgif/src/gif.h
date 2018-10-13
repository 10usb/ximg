#ifndef GIF_H
#define GIF_H

/**
 * GIf file format defenitions
 */

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

#endif