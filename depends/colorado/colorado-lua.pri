SOURCES += \
	$$PWD/gl-buffer.cpp \
	$$PWD/gl-shader.cpp \
	$$PWD/plane.cpp \
	$$PWD/triangle-optimizer.cpp \
	$$PWD/ui-vbo-node.cpp

HEADERS  += \
	$$PWD/gl-buffer.h \
	$$PWD/gl-shader.h \
	$$PWD/plane.h \
	$$PWD/triangle-optimizer.h \
	$$PWD/ui-vbo-element.h \
	$$PWD/ui-vbo-node.h

# Headers with no associated code
HEADERS += \
	$$PWD/colorado-gl.h

# PhysFS wrappers
SOURCES += \
	$$PWD/read-file-physfs.cpp \

HEADERS  += \
	$$PWD/read-file-physfs.h \

# Lua bindings
SOURCES += \
	$$PWD/lua-bindings.cpp \
	$$PWD/lua-common.cpp \
	$$PWD/lua-matrix.cpp \
	$$PWD/lua-sdl.cpp \
	$$PWD/lua-shader.cpp \

HEADERS += \
	$$PWD/lua-bindings.h \
	$$PWD/lua-common.h \
	$$PWD/lua-matrix.h \
	$$PWD/lua-sdl.h \
	$$PWD/lua-shader.h \
