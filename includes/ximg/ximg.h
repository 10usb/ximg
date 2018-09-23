#ifndef XIMG_H
#define XIMG_H

/**
 * The Ximg file format is flexible by design. The base is no more and no less
 * then a header followed by chunks that can vary in data and size. The actual
 * content can be found by special chunks that define the contents.
 */

#include <stdint.h>

/**
 * File header
 */
struct ximg_header {
    uint8_t fileid[8];  // 8 bytes
    uint32_t size;      // 4 bytes
    uint32_t checksum;  // 4 bytes
    uint16_t chunks;    // 2 bytes
    uint16_t flags;     // 2 bytes
} __attribute__((__packed__));


/**
 * Identifier type of chunks
 */
typedef uint32_t ximgid_t;

/**
 * Identifier of types
 */
typedef uint32_t ximgtype_t;

/**
 * Chunk
 */
struct xchu {
    uint32_t size;      // 4 bytes
    ximgtype_t type;    // 4 bytes
    ximgid_t id;        // 4 bytes
    uint32_t checksum;  // 4 bytes
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
 * Extracts an image from a memory buffer
 */
struct ximg * ximg_extract(const void * source, uint32_t size);

/**
 * Dumps the image to a memory buffer
 */
int ximg_dump(struct ximg * image, void * destination, uint32_t size);

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
ximgid_t ximg_add(struct ximg * image, uint32_t size, ximgtype_t type);

/**
 * Returns a chunk with the given id if it exists, otherwise returns 0
 */
struct xchu * ximg_get(struct ximg * image, ximgid_t id);

/**
 * Find a chunk of given type at given offset otherwise 0
 */
struct xchu * ximg_find(struct ximg * image, ximgtype_t type, uint16_t offset);

/**
 * Converts a string to a number type identifier
 */
ximgtype_t ximg_make(const char * type);

/**
 * Retruns the contents of a chunk
 */
void * xchu_contents(struct xchu * chunk);

#endif
