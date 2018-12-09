# Ximg converter
include ../../makefile.in
OUTPUT=bin/ximg
SOURCES=main.c info.c append.c extract.c remove.c translate.c
DEPENDANCIES=libximg libxras
TEMP=obj
OBJECTS=$(SOURCES:%.c=$(TEMP)/%.o)
ifeq ($(OS),Windows_NT)
    TARGET=$(OUTPUT).exe
else
	TARGET=$(OUTPUT)
endif
CFLAGS=-I$(INCLUDES)
LIBS=$(foreach x, $(DEPENDANCIES), $(ROOT)obj/$(x).o)

build: $(TARGET)

rebuild: clean dependencies $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(LIBS) | $(dir $(TARGET))
	@echo "$(CC) -Wall -o $@ $(OBJECTS) <$(DEPENDANCIES)>"
	@$(CC) -Wall -o $@ $(OBJECTS) $(LIBS)

$(TEMP)/%.o: src/%.c | $(TEMP)
	$(CC) -c $(CFLAGS) -o $@ $<

$(TEMP) $(dir $(TARGET)):
	mkdir $@

$(LIBS):
	@$(MAKE) --no-print-directory -C $(@:$(ROOT)obj/%.o=$(ROOT)%) OUTPUT=$@

dependencies:
	@$(foreach x,$(DEPENDANCIES),$(MAKE) --no-print-directory -C $(ROOT)$(x) OUTPUT=$(ROOT)obj/$(x).o;)

.PHONY: build rebuild clean dependencies