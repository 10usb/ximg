# Ximg

Why Ximg? Back in the days I had a lot of frustration with
readin/writing/mutating images. As my goal was to ease the creation of icon
files. There have been many concepts. Each time a part had been improved, but it
never set foot. Until I had a purpose for it, where became a tool instead of the
main project. And the code became a working whole. Now one might question what
does the X mean in Ximg. I don't know. When I started with the format I still
was in the delusion appeding X to a name would look cool :P

## Concept
The concept of the format is to be as flexible as posible. Allowing multiple
images to be saved in a single file. Even allow the images to share the same
resources within the file. This is accomplished by splitting the contents in
small containers that have no direct interaction with each other unless
specified.

## File support
 - Bitmap (.bmp) - with 24bit or 32bit colors
 - Graphics Interchange Format (.gif) - only single image decoding

## Creating a Ximg
```c
// Empty image in memory
struct ximg * image = ximg_create();

// Create a 24bit RGB raster image
ximgid_t raster_id = xras_create(image, 150, 100, ximg_make("RGB8"), 3);

// Get the memory pointer to that raster image
struct xras * raster = xras_get_by_id(image, raster_id);

// Extract the pointers to each of the color channels
struct xchan * r = xras_channel(image, raster, 0);
struct xchan * g = xras_channel(image, raster, 1);
struct xchan * b = xras_channel(image, raster, 2);

// Set some pretty colors
for(int y = 0; y < 100; y++){
    for(int y = 0; y < 150; y++){
        xchan_set8(r, x, y, (x * 256 / 150) % 256);
        xchan_set8(g, x, y, (y * 256 / 100) % 256);
        xchan_set8(b, x, y, ((x * 256 / 150)  + (y * 256 / 100)) % 256);
    }
}

// Save it to disk
ximg_save(image, "pretty-image.ximg");

// We don't like leaks
ximg_free(image);
```

## Loading and reading an Ximg
```c
// Load the image from file
struct ximg * image = ximg_load("pretty-image.ximg");
if(image){
    struct xreader reader;
    // Init a reader loaded with the first raster image (index 0)
    if(!xreader_init(&reader, image, 0)) return 0;

    // Now tell the reader we want a pixel at position left,top 50,50
    struct xpixel xpixel;
    xreader_rgba(&reader, 50, 50, &xpixel);

    // xpixel.r = 0x55
    // xpixel.g = 0x80
    // xpixel.b = 0xD5
    
    // Make sure any memory allocated by the reader is made free
    xreader_clear(&reader);
    ximg_free(image);
}
```

## Wishes to be added
 - Full Bitmap support
 - Conversion of 24bit to an mapped color image using dithering
 - Animated GIF
 - Loading and saving of Portable Network Graphics (.png)
 - Loading of Joint Photographic Experts Group (.jpeg, .jpg)