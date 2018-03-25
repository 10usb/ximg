# Library Ximg
include ../makefile.in
OUTPUT=bin/libximg.o
SOURCES=ximg.c io.c
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