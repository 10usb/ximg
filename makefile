# Workspace
OUTPUT=bin
TEMP=obj
#TOOLS=xbitmap ximg
TOOLS=xgif
#LIBS=libxbitmap libximg libxras
LIBS=liblzw

TARGETS=$(TOOLS:%=$(OUTPUT)/%)
OBJECTS=$(LIBS:%=$(TEMP)/%)

build: $(OBJECTS) $(TARGETS)

rebuild: clean $(OBJECTS) $(TARGETS)

all: $(LIBS) $(TOOLS)

clean:
	$(foreach x,$(LIBS),$(MAKE) -C $(x) clean;)
	$(foreach x,$(TOOLS),$(MAKE) -C tools/$(x) clean;)
	rm -rf $(TARGETS) $(OBJECTS)

$(TARGETS): $(OUTPUT) $(OBJECTS)
	@echo "Building $(subst $(OUTPUT)/,,$@)"
	@$(MAKE) --no-print-directory -C tools/$(subst $(OUTPUT)/,,$@) build OUTPUT=$(abspath $@)

$(OBJECTS): $(TEMP)
	@echo "Building $(subst $(TEMP)/,,$@)"
	@$(MAKE) --no-print-directory -C $(subst $(TEMP)/,,$@) build OUTPUT=$(abspath $@).o

$(OUTPUT) $(TEMP):
	mkdir $@

.PHONY: build rebuild all clean $(TOOLS) $(LIBS)