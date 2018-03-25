# Library Ximg Bitmap
include ../makefile.in
OUTPUT=bin/libxbitmap.o
SOURCES=xbitmap.c
TEMP=obj

OBJECTS=$(SOURCES:%.c=$(TEMP)/%.o)
CFLAGS=-I$(INCLUDES)

build: $(OUTPUT)

rebuild: clean $(OUTPUT)

clean:
	$(RM) $(OUTPUT) $(OBJECTS)

$(OUTPUT): $(dir $(OUTPUT)) $(TEMP) $(OBJECTS)
	$(LD) -i -o $@ $(OBJECTS)

$(TEMP)/%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(TEMP) $(dir $(OUTPUT)):
	mkdir $@

.PHONY: build rebuild clean