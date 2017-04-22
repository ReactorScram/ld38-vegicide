default: all

MODULES = graphics platformer

.PHONY: clean debug all $(MODULES)

all:
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) &&) true

clean:
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) clean &&) true

debug:
	$(foreach MODULE, $(MODULES), $(MAKE) -C $(MODULE) debug && ) true
