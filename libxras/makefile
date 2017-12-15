# Library Ximg Raster
include ../makefile.in
SOURCES=xras.c xchan.c xreader.c xwriter.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=libxras.o
INCLUDES=-I../includes

$(TARGET): $(OBJECTS)
	$(LD) -i -o $@ $(OBJECTS)

%.o: %.c
	$(CC) -c $(INCLUDES) $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

all: $(TARGET)

.PHONY: clean all
