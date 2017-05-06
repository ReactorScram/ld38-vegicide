all: gl.js animate.js

.PHONY: all

gl.js: gl.ts webgl.d.ts
	tsc --lib es6 $<

animate.js: animate.cpp
	em++ --std=c++1y -s EXPORTED_FUNCTIONS="['_animate']" -g0 -O2 -o $@ $<
