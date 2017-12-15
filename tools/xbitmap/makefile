# Ximg bitmap converter
include ../../makefile.in
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
ifeq ($(OS),Windows_NT)
    TARGET=xbitmap.exe
else
	TARGET=xbitmap
endif
INCLUDES=-I../../includes
DEPENDANCIES=libximg libxras libxbitmap
LIBS=$(foreach x, $(DEPENDANCIES), ../../$(x)/$(x).o)

all: dependencies $(TARGET)

$(TARGET): $(OBJECTS) $(LIBS)
	$(CC) -Wall -o $@ $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) -c $(INCLUDES) $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

dependencies:
	$(foreach x,$(DEPENDANCIES:%=../../%),$(MAKE) -C $(x);)


.PHONY: clean all dependencies
