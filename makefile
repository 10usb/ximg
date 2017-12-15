# Workspace
TOOLS=xbitmap ximg
LIBS=libxbitmap libximg libxras
ifeq ($(OS),Windows_NT)
    TARGETS=$(TOOLS:%=bin/%.exe)
	SUFFIX=.exe
else
	TARGETS=$(TOOLS:%=bin/%)
	SUFFIX=
endif

all: $(LIBS) $(TOOLS)

clean:
	$(foreach x,$(LIBS),$(MAKE) -C $(x) clean;)
	$(foreach x,$(TOOLS),$(MAKE) -C tools/$(x) clean;)
	rm -rf bin

bin:
	mkdir bin

$(LIBS):
	$(MAKE) -C $@

$(TOOLS): bin
	$(MAKE) -C tools/$@ $@$(SUFFIX)
	cp tools/$@/$@$(SUFFIX) bin/$@$(SUFFIX)

.PHONY: all clean $(TOOLS) $(LIBS)