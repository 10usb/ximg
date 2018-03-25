# Workspace
OUTPUT=bin
TEMP=obj
#TOOLS=xbitmap ximg
TOOLS=xgif
LIBS=libximg libxras libxbitmap liblzw

TARGETS=$(TOOLS:%=$(OUTPUT)/%)
OBJECTS=$(LIBS:%=$(TEMP)/%.o)

build: $(OBJECTS) $(TARGETS)

rebuild: clean $(OBJECTS) $(TARGETS)

clean:
	#$(foreach x,$(LIBS),$(MAKE) -C $(x) clean;)
	#$(foreach x,$(TOOLS),$(MAKE) -C tools/$(x) clean;)
	rm -rf $(TARGETS) $(OBJECTS)

$(TARGETS): $(OBJECTS)
	@echo "Building $(subst $(OUTPUT)/,,$@)"
	@$(MAKE) --no-print-directory -C tools/$(subst $(OUTPUT)/,,$@) build OUTPUT=$(abspath $@)

$(OBJECTS):
	@echo "Building $(subst $(TEMP)/,,$@)"
	@$(MAKE) --no-print-directory -C $(@:$(TEMP)/%.o=%) build OUTPUT=$(abspath $@)

.PHONY: build rebuild clean $(TARGETS)