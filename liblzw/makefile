# Library Lempel–Ziv–Welch
include ../makefile.in
OUTPUT=bin/liblzw.o
SOURCES=lzw.c
TEMP=obj

OBJECTS=$(SOURCES:%.c=$(TEMP)/%.o)
CFLAGS=-I$(INCLUDES)

build: $(OUTPUT)

rebuild: clean $(OUTPUT)

clean:
	$(RM) $(OUTPUT) $(OBJECTS)

$(OUTPUT): $(OBJECTS) | $(dir $(OUTPUT))
	$(LD) -i -o $@ $(OBJECTS)

$(TEMP)/%.o: src/%.c | $(TEMP)
	$(CC) -c $(CFLAGS) -o $@ $<

$(TEMP) $(dir $(OUTPUT)):
	mkdir $@

.PHONY: build rebuild clean