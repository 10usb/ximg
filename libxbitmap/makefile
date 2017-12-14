# Library Ximg Bitmap
include ../makefile.in
SOURCES=xbitmap.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=libxbitmap.o
INCLUDES=-I../includes

$(TARGET): $(OBJECTS)
	$(LD) -i -o $@ $(OBJECTS)

%.o: %.c
	$(CC) -c $(INCLUDES) $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

all: $(TARGET)

.PHONY: clean all
