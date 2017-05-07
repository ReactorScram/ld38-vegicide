all: gl.js animate.js

.PHONY: all

gl.js: gl.ts webgl.d.ts
	tsc --lib es6 $<

INC_DIR = -I.. -I../../glm-0.9.8.4/glm

# -s ASSERTIONS=2

animate.js: animate.cpp
	em++ --std=c++1y -s EXPORTED_FUNCTIONS="['_vegicide_init', '_vegicide_process_input', '_vegicide_step', '_vegicide_get_graphics_json']" -g0 -Os $(INC_DIR) -s ASYNCIFY=1 -Werror -o $@ $<
