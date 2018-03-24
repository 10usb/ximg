# Ximg gif converter
include ../../makefile.in
OUTPUT=bin/xgif
SOURCES=main.c lzw.c
DEPENDANCIES=libximg libxras libxbitmap
TEMP=obj
OBJECTS=$(SOURCES:%.c=$(TEMP)/%.o)
ifeq ($(OS),Windows_NT)
    TARGET=$(OUTPUT).exe
else
	TARGET=$(OUTPUT)
endif
CFLAGS=-I$(INCLUDES)
LIBS=$(foreach x, $(DEPENDANCIES), $(ROOT)$(x)/$(x).o)
all: dependencies $(TARGET)

$(TARGET): $(dir $(TARGET)) $(TEMP) $(OBJECTS) $(LIBS)
	@echo "$(CC) -Wall -o $@ $(OBJECTS) <$(DEPENDANCIES)>"
	@$(CC) -Wall -o $@ $(OBJECTS) $(LIBS)

$(TEMP)/%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(TEMP) $(dir $(TARGET)):
	mkdir $@

clean:
	$(RM) $(TARGET) $(OBJECTS)

dependencies:
	@$(foreach x,$(DEPENDANCIES:%=$(ROOT)%),$(MAKE) --no-print-directory -C $(x);)


.PHONY: clean all dependencies
