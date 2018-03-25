# Workspace
OUTPUT=bin
TEMP=obj
#TOOLS=xbitmap ximg
TOOLS=xgif
LIBS=libximg libxras libxbitmap liblzw

ifeq ($(OS),Windows_NT)
    SUFFIX=.exe
else
	SUFFIX=
endif
TARGETS=$(TOOLS:%=$(OUTPUT)/%$(SUFFIX))
OBJECTS=$(LIBS:%=$(TEMP)/%.o)

build: $(OBJECTS) $(TARGETS)

rebuild: clean build

clean:
	rm -rf $(TARGETS) $(OBJECTS)

clean-all:
	@$(foreach x,$(OBJECTS),$(MAKE) --no-print-directory -C $(x:$(TEMP)/%.o=%) clean OUTPUT=$(abspath $(x));)
	@$(foreach x,$(TARGETS),$(MAKE) --no-print-directory -C tools/$(x:$(OUTPUT)/%$(SUFFIX)=%) clean OUTPUT=$(abspath $(x:%$(SUFFIX)=%));)

rebuild-all: clean-all build

check:
	@$(foreach x,$(OBJECTS),$(MAKE) --no-print-directory -C $(x:$(TEMP)/%.o=%) build OUTPUT=$(abspath $(x));)
	@$(foreach x,$(TARGETS),$(MAKE) --no-print-directory -C tools/$(x:$(OUTPUT)/%$(SUFFIX)=%) build OUTPUT=$(abspath $(x:%$(SUFFIX)=%));)

$(OBJECTS):
	@echo "Building $(@:$(TEMP)/%.o=%)"
	@$(MAKE) --no-print-directory -C $(@:$(TEMP)/%.o=%) build OUTPUT=$(abspath $@)

$(TARGETS): $(OBJECTS)
	@echo "Building $(@:$(OUTPUT)/%$(SUFFIX)=%)"
	@$(MAKE) --no-print-directory -C tools/$(@:$(OUTPUT)/%$(SUFFIX)=%) build OUTPUT=$(abspath $(@:%$(SUFFIX)=%))

.PHONY: build rebuild clean clean-all rebuild-all check