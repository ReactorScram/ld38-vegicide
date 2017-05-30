SOURCES += \
	$$PWD/aabb.cpp \
	$$PWD/attribute-enabler.cpp \
	$$PWD/audio-mixer.cpp \
	$$PWD/camera.cpp \
	$$PWD/fixed-timestep.cpp \
	$$PWD/game.cpp \
	$$PWD/gl.cpp \
	$$PWD/gl-buffer.cpp \
	$$PWD/gl-error.cpp \
	$$PWD/gl-shader.cpp \
	$$PWD/lua-5.1-helper.cpp \
	$$PWD/lua-state.cpp \
	$$PWD/mesh.cpp \
	$$PWD/mesh-binder.cpp \
	$$PWD/meshLua.cpp \
	$$PWD/node.cpp \
	$$PWD/node-renderer.cpp \
	$$PWD/obj-loader.cpp \
	$$PWD/particle-element.cpp \
	$$PWD/particle-node.cpp \
	$$PWD/plane.cpp \
	$$PWD/read-file.cpp \
	$$PWD/screen-options.cpp \
	$$PWD/sky-box.cpp \
	$$PWD/sub-node.cpp \
	$$PWD/triangle-optimizer.cpp \
	$$PWD/triangle-shader.cpp \
	$$PWD/ui-vbo-node.cpp \
	$$PWD/vbo.cpp \
	$$PWD/vbo-node.cpp \
	$$PWD/vbo-element.cpp \

INC_DIR = $$PWD/colorado

HEADERS  += \
	$$INC_DIR/aabb.h \
	$$INC_DIR/attribute-enabler.h \
	$$INC_DIR/audio-mixer.h \
	$$INC_DIR/camera.h \
	$$INC_DIR/fixed-timestep.h \
	$$INC_DIR/game.h \
	$$INC_DIR/gl.h \
	$$INC_DIR/gl-buffer.h \
	$$INC_DIR/gl-error.h \
	$$INC_DIR/gl-shader.h \
	$$INC_DIR/lua-5.1-helper.h \
	$$INC_DIR/lua-state.h \
	$$INC_DIR/mesh.h \
	$$INC_DIR/mesh-binder.h \
	$$INC_DIR/node.h \
	$$INC_DIR/node-renderer.h \
	$$INC_DIR/obj-loader.h \
	$$INC_DIR/particle-element.h \
	$$INC_DIR/particle-node.h \
	$$INC_DIR/plane.h \
	$$INC_DIR/read-file.h \
	$$INC_DIR/screen-options.h \
	$$INC_DIR/sky-box.h \
	$$INC_DIR/sub-node.h \
	$$INC_DIR/triangle-optimizer.h \
	$$INC_DIR/triangle-shader.h \
	$$INC_DIR/ui-vbo-element.h \
	$$INC_DIR/ui-vbo-node.h \
	$$INC_DIR/vbo.h \
	$$INC_DIR/vbo-element.h \
	$$INC_DIR/vbo-node.h \

# Texture loading
SOURCES += \
	$$PWD/image-to-texture.cpp \

HEADERS  += \
	$$INC_DIR/image-to-texture.h \

# Lua bindings
COLORADO_USE_LUA_BINDINGS {
	SOURCES += \
		$$PWD/game-lua.cpp \
		$$PWD/lua/colorado.cpp \
		$$PWD/lua/input.cpp \
		$$PWD/lua/lua-audio.cpp \
		$$PWD/lua/lua-buffer.cpp \
		$$PWD/lua/lua-game.cpp \
		$$PWD/lua/lua-matrix.cpp \
		$$PWD/lua/lua-sdl.cpp \
		$$PWD/lua/lua-ui-vbo-node.cpp \
		$$PWD/lua/matrix.cpp \
		$$PWD/lua/shader.cpp
	
	HEADERS += \
		$$INC_DIR/game-lua.h \
		$$INC_DIR/lua/colorado.h \
		$$INC_DIR/lua/input.h \
		$$INC_DIR/lua/lua-audio.h \
		$$INC_DIR/lua/lua-buffer.h \
		$$INC_DIR/lua/lua-game.h \
		$$INC_DIR/lua/lua-gl.h \
		$$INC_DIR/lua/lua-matrix.h \
		$$INC_DIR/lua/lua-sdl.h \
		$$INC_DIR/lua/lua-ui-vbo-node.h \
		$$INC_DIR/lua/matrix.h \
		$$INC_DIR/lua/shader.h
}
