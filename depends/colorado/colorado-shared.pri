TARGET = colorado

TEMPLATE = lib

CONFIG -= QT
QT -= core gui
CONFIG += shared
CONFIG += COLORADO_USE_LUA_BINDINGS c++14

DEFINES += GLM_FORCE_CXX98

COLORADO_USE_LUAJIT {
	DEFINES += COLORADO_USE_LUAJIT
}

OBJECTS_DIR = ./obj

COLORADO_PANDORA {

}
else {
	DEFINES += USE_GLEW USE_DESKTOP_GL
}

CONFIG += link_pkgconfig

COLORADO_DEPENDENCIES=".."
WIN_DEPS = "Z:/home/user/projects-windows"

# EglPort
COLORADO_PANDORA {
	EGLPORT_ROOT = $$COLORADO_DEPENDENCIES/eglport
	
	INCLUDEPATH += $$EGLPORT_ROOT
	DEPENDPATH += $$EGLPORT_ROOT
	HEADERS += $$EGLPORT_ROOT/eglport.h
	SOURCES += $$EGLPORT_ROOT/eglport.c
	
	DEFINES += COLORADO_HAVE_GLES USE_EGL_SDL USE_GLES2
	
	LIBS += -lEGL
	LIBS += -lGLESv2
}

# GetOpt_pp
INCLUDEPATH += "$$COLORADO_DEPENDENCIES/getoptpp"
HEADERS += $$COLORADO_DEPENDENCIES/getoptpp/getoptpp/getopt_pp.h \
	colorado/dllspec.h
SOURCES += $$COLORADO_DEPENDENCIES/getoptpp/src/getopt_pp.cpp

# GLee
win32 {
	INCLUDEPATH += "$$COLORADO_DEPENDENCIES/glee/include/GL"
	LIBS += -L"$$COLORADO_DEPENDENCIES/build-glee-Desktop_Qt_5_6_0_MinGW_32bit-Default" -lglee
}
else:COLORADO_PANDORA {
	
}
else {
	LIBS += -lGLee
}

# GLM
COLORADO_PANDORA {
	INCLUDEPATH += "$$COLORADO_DEPENDENCIES/glm-0.9.6.1/glm"
	DEPENDPATH += "$$COLORADO_DEPENDENCIES/glm-0.9.6.1/glm"
}
else: win32 {
	INCLUDEPATH += "$$COLORADO_DEPENDENCIES/glm-0.9.6.1/glm"
}
else: COLORADO_DEBIAN {
	INCLUDEPATH += "$$COLORADO_DEPENDENCIES/glm-0.9.6.3/glm"
}
else {
	# Installed through package manager
}

# Lua
COLORADO_PANDORA {
	INCLUDEPATH += "$$COLORADO_DEPENDENCIES/LuaJIT-2.0.3/src"
	LIBS += -L"$$COLORADO_DEPENDENCIES/LuaJIT-2.0.3/src"
}
else:win32 {
	INCLUDEPATH += "$$WIN_DEPS/LuaJIT-2.0.4/src"
	LIBS += -L"$$WIN_DEPS/LuaJIT-2.0.4/src" -llua51
}
else: COLORADO_DEBIAN {
	INCLUDEPATH += "$$COLORADO_DEPENDENCIES/LuaJIT-2.0.4/src"
	LIBS += -L"$$COLORADO_DEPENDENCIES/LuaJIT-2.0.4/src"
}
else {
	PKGCONFIG += luajit
}

# SDL
win32 {
	INCLUDEPATH += "$$WIN_DEPS/SDL-1.2.15/include"
	INCLUDEPATH += "$$WIN_DEPS/SDL-1.2.15/include/SDL"
	LIBS += -L"$$WIN_DEPS/SDL-1.2.15/lib" -lSDL
}
else:COLORADO_PANDORA {
	INCLUDEPATH += /mnt/utmp/codeblocks/usr/include/SDL
	# Don't use the Code::Blocks SDL
	LIBS += -lSDL -lpthread
}
else: COLORADO_DEBIAN {
	INCLUDEPATH += "$$COLORADO_DEPENDENCIES/SDL_mixer-1.2.12"
	LIBS += -L"$$COLORADO_DEPENDENCIES/SDL_mixer-1.2.12/build"
	LIBS += -lSDL_mixer
}
else {
	PKGCONFIG += sdl
}

# SDL_mixer
win32 {
	INCLUDEPATH += "$$WIN_DEPS/SDL_mixer-1.2.12/include"
	LIBS += -L"$$WIN_DEPS/SDL_mixer-1.2.12/lib/x86" -lSDL_mixer
}
else:COLORADO_PANDORA {
	
}

# SOIL

win32|COLORADO_PANDORA {
	COLORADO_SOIL_PATH="$$COLORADO_DEPENDENCIES/soil/src"
	
	INCLUDEPATH += "$$COLORADO_SOIL_PATH"
	SOURCES += \
		$$COLORADO_SOIL_PATH/SOIL.c \
		$$COLORADO_SOIL_PATH/image_DXT.c \
		$$COLORADO_SOIL_PATH/image_helper.c \
		$$COLORADO_SOIL_PATH/stb_image_aug.c

	LIBS += -lOpenGL32
}
else {
	INCLUDEPATH += /usr/include/SOIL
	LIBS += -lSOIL
}

# TERF
INCLUDEPATH += $$COLORADO_DEPENDENCIES/terf
win32 {
	debug {
		LIBS += -L$$COLORADO_DEPENDENCIES/terf/build-terf-Desktop_Qt_5_6_0_MinGW_32bit-Debug/debug
	}
	release {
		LIBS += -L$$COLORADO_DEPENDENCIES/terf/build-terf-Desktop_Qt_5_6_0_MinGW_32bit-Release/release
	}
	
	LIBS += -lterf
}
else: COLORADO_PANDORA {
	LIBS += -L$$COLORADO_DEPENDENCIES/terf/terf -lterf
}
else: COLORADO_DEBIAN {
	
}
else {
	debug {
		LIBS += -L$$COLORADO_DEPENDENCIES/terf/build-terf-Desktop-Debug/
	}
	else {
		LIBS += -L$$COLORADO_DEPENDENCIES/terf/build-terf-Desktop-Release/
	}
}

# All the source files and headers
include ("colorado.pri")
