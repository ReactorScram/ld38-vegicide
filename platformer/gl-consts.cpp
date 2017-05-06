#include "gl-consts.h"

// This is needed for scene.cpp to create the render passes
// I didn't want it to pollute the Emscripten code, because
// that code should be effectively headless (I already rewrote
// the graphics module in TypeScript so I don't want to deal
// with Emscripten's concept of OpenGL

#include "colorado/gl.h"

const int VEGL_DEPTH_TEST = GL_DEPTH_TEST;
const int VEGL_CULL_FACE = GL_CULL_FACE;
const int VEGL_BLEND = GL_BLEND;
const int VEGL_SRC_ALPHA = GL_SRC_ALPHA;
const int VEGL_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA;
const int VEGL_DST_COLOR = GL_DST_COLOR;
const int VEGL_ZERO = GL_ZERO;
