# Workspace
ifeq ($(OS),Windows_NT)
    XBITMAP=bin/xbitmap.exe
	XBITMAP_SOURCE=tools/xbitmap/xbitmap.exe
else
	XBITMAP=bin/xbitmap
	XBITMAP_SOURCE=tools/xbitmap/xbitmap
endif
TARGETS=$(XBITMAP)
LIBS=libxbitmap libximg libxras
TOOLS=xbitmap

all: $(TARGETS)

clean:
	$(foreach x,$(LIBS),$(MAKE) -C $(x) clean;)
	$(foreach x,$(TOOLS),$(MAKE) -C tools/$(x) clean;)
	rm -rf bin

bin:
	mkdir bin

$(XBITMAP): bin
	$(MAKE) -C tools/xbitmap
	cp $(XBITMAP_SOURCE) $(XBITMAP)

.PHONY: all clean