default: all

MODULES = graphics platformer

.PHONY: clean debug all $(MODULES)

all: maps/demo.bin
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) &&) true

clean:
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) clean &&) true

debug:
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) debug && ) true

maps/demo.bin: maps/demo.lua
	luajit map-binner.lua --tiles $< > $@

maps/demo.sql: maps/demo.lua
	luajit map-binner.lua --objects $< $@
