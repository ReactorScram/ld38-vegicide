default: all

MODULES = depends graphics audio platformer 

DEP_PATH = $(realpath depends)

.PHONY: clean debug all $(MODULES) colorado

all: maps/demo.bin maps/demo.sqlite colorado
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) DEP_PATH=$(DEP_PATH) &&) true

clean:
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) clean &&) true

debug:
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) debug && ) true

maps/demo.bin: maps/demo.lua
	luajit map-binner.lua --tiles $< > $@

maps/demo.sqlite: maps/demo.lua
	luajit map-binner.lua --objects $< | sqlite3 $@
