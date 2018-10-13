#ifndef GIF_H
#define GIF_H

/**
 * GIf file format defenitions
 */

struct xgif_header {
	uint16_t width;
	uint16_t height;
	union {
		uint8_t flags;
		struct {
			uint8_t hasTable : 1;
			uint8_t colorResolution : 3;
			uint8_t sortFlag : 1;
			uint8_t tableSize : 3;
		} __attribute__((__packed__));
	};
	uint8_t backgroundIndex;
	uint8_t pixelRatio;
} __attribute__((__packed__));

struct xgif_fragment {
	uint16_t left;
	uint16_t top;
	uint16_t width;
	uint16_t height;
	union {
		uint8_t flags;
		struct {
			uint8_t hasTable : 1;
			uint8_t interlaced : 1;
			uint8_t sortFlag : 1;
			uint8_t reserved : 2;
			uint8_t tableSize : 3;
		} __attribute__((__packed__));
	};
	unsigned char minimumCodeSize;
} __attribute__((__packed__));

struct xgif_extension {
	uint8_t type;
	uint8_t size;
} __attribute__((__packed__));

struct xgif_graphic_control {
	union {
		uint8_t flags;
		struct {
			uint8_t reserved : 3;
			uint8_t disposalMethod : 3;
			uint8_t userInput : 1;
			uint8_t transparent : 1;
		} __attribute__((__packed__));
	};
	uint16_t delay;
	uint8_t transparentIndex;
};

#endif