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