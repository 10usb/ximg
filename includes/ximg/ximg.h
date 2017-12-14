#ifndef XIMG_H
#define XIMG_H

/**
 * The Ximg file format is flexible by design. The base is no more and no less
 * then a header followed by chunks that can vary in data. The actual content
 * can be found by special chunks that define the contents.
 */

/**
 * File header
 */
struct ximg_header {
    char fileid[8];         // 8 bytes
    unsigned int size;      // 4 bytes
    unsigned int checksum;  // 4 bytes
    unsigned short chunks;  // 2 bytes
    unsigned short flags;   // 2 bytes
} __attribute__((__packed__));

/**
 * Chunk
 */
struct xchu {
    unsigned int size;      // 4 bytes
    unsigned int type;      // 4 bytes
    unsigned int id;        // 4 bytes
    unsigned int checksum;  // 4 bytes
};

/**
 * Handle in memory
 */
struct ximg {
    struct ximg_header header; // 20 bytes
    struct xchu ** chunks;
};

/**
 * Creates an image
 */
struct ximg * ximg_create();

/**
 * Free's all memory associated with this image 
 */
void ximg_free(struct ximg * image);

/**
 * Loads an image from a file
 */
struct ximg * ximg_load(const char * filename);

/**
 * Saves the image to a file
 */
int ximg_save(struct ximg * image, const char * filename);

/**
 * Adds a new chunk to the image with the given size en type
 * @remarks type can be generated with ximg_make function
 */
unsigned int ximg_add(struct ximg * image, unsigned int size, unsigned int type);

/**
 * Returns a chunk with the given id if it exists, otherwise returns 0
 */
struct xchu * ximg_get(struct ximg * image, unsigned int id);

/**
 * Find a chunk of given type at given offset otherwise 0
 */
struct xchu * ximg_find(struct ximg * image, unsigned int type, unsigned short offset);

/**
 * Converts a string to a number type identifier
 */
unsigned int ximg_make(const char * type);

/**
 * Retruns the contents of a chunk
 */
void * xchu_contents(struct xchu * chunk);

#endif
