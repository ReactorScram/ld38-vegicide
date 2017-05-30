CONFIG -= QT

TARGET = colorado-host

TEMPLATE = app

OBJECTS_DIR = obj/

# Libraries

# Colorado
# Has to come first so it can link with PhysFS
DEPENDPATH += .
win32 {
	LIBS += -L../build-colorado-shared-windows-Desktop_Qt_5_1_1_MinGW_32bit-Release/release
}
else {
	LIBS += -L../colorado
}

LIBS += -lcolorado

COLORADO_PANDORA {
	QMAKE_CXXFLAGS_RELEASE += -O2
	
	CONFIG += link_pkgconfig
	
	# Colorado Pandora defines
	DEFINES += COLORADO_HAVE_GLES COLORADO_PANDORA USE_EGL_SDL
	
	# Lua
	LIBS += -L/mnt/utmp/codeblocks/usr/lib/ -lluajit-5.1
	
	LIBS += -ldl	
	
	# OpenGL ES 2
	LIBS += -lEGL -lGLESv2
	
	# no Qt
	QT -= core gui
	
	# SOIL
	INCLUDEPATH += "../soil/src"
	LIBS += -L"../soil/lib" -lSOIL
}
else {
	# Colorado desktop defines
	DEFINES += USE_GLEW USE_DESKTOP_GL
	
	win32 {
		# GLEW
		INCLUDEPATH += "../glew-1.10.0/include"
		LIBS += -L"../glew-1.10.0/lib" -lglew
		LIBS += -lOpenGL32
		
		# Lua 5.2.3
		INCLUDEPATH += "../lua-5.2.3/src"
		LIBS += -L"../lua-5.2.3/src" -llua52
		
		# PhysFS
		INCLUDEPATH += "../physfs-2.0.3"
		DEPENDPATH += "../physfs-2.0.3"
		
		# SOIL
		INCLUDEPATH += "../soil/src"
		LIBS += -L"../soil/lib" -lSOIL -lOpenGL32
	}
	else {
		# GNU / Linux
		
		# SDL, Lua, GLEW
		CONFIG += link_pkgconfig
		PKGCONFIG += luajit glew
		
		# PhysFS
		LIBS += -lphysfs
		
		# SOIL
		INCLUDEPATH += "../soil/src"
		LIBS += -L"../soil/lib" -lSOIL
		
		LIBS += -lGL
	}
}

# SDL, SDL_mixer
COLORADO_PANDORA {
	
}
else {
	win32 {
		INCLUDEPATH += "../SDL-1.2.15/include"
		LIBS += -L../SDL-1.2.15/bin -L../SDL-1.2.15/lib -lSDL
		
		INCLUDEPATH += "../SDL_mixer-1.2.12/include"
	}
	else {
		PKGCONFIG += sdl
	}
}

# GLM, from source
INCLUDEPATH += "../glm-0.9.4.6/glm"

SOURCES += colorado-host-main.cpp
