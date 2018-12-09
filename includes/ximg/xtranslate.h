#ifndef XTRANSLATE_H
#define XTRANSLATE_H

/**
 * To translate a mapped image to a raster image or from a raster to a mapped
 */

#include <ximg/ximg.h>
#include <ximg/xpal.h>

/**
 * Translates a mapped image into a raster image
 * @param src   source image to get the mapped image data
 * @param id    id of the mapped image
 * @param dest  destination image to insert raster image
 * @return      id of the raster image
 */
ximgid_t xtranslate_mapped(struct ximg * src, ximgid_t id, struct ximg * dest);

/**
 * Translates a raster image into a mapped image
 * @param src   source image to get the raster image data
 * @param id    id of the raster image
 * @param dest  destination image to insert mapped image
 * @return      id of the mapped image
 */
ximgid_t xtranslate_raster(struct ximg * src, ximgid_t id, struct ximg * dest);

/**
 * Translates a raster image into a mapped image using the given palette
 * @param src       source image to get the raster image data
 * @param id        id of the raster image
 * @param dest      destination image to insert mapped image
 * @param palette   index of the palette to use (must exists in de destination)
 * @return          id of the mapped image
 */
ximgid_t xtranslate_raster_with_palette(struct ximg * src, ximgid_t id, struct ximg * dest, ximgid_t palette_id);

#endif
