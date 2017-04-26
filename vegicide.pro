TEMPLATE = app
TARGET = Vegicide_win32

CONFIG -= app_bundle
CONFIG -= QT
QT -= core gui

DEP_PATH = Z:/home/user/projects
WIN_DEPS= Z:/home/user/projects-windows

DEFINES += TERF_LIBRARY

SOURCES += \
	$$PWD/audio/audio.cpp \
	$$PWD/graphics/gl-state-tracker.cpp \
	$$PWD/graphics/shader-binder.cpp \
	$$PWD/graphics/texture-binder.cpp \
	$$PWD/graphics/graphics.cpp \
	$$PWD/graphics/mesh-binder.cpp \
	$$PWD/graphics/ecs.cpp \
	$$PWD/platformer/main.cpp \
	$$PWD/platformer/scene.cpp \
	$$PWD/platformer/level.cpp \
	$$PWD/platformer/logic.cpp \
	$$PWD/platformer/input.cpp \
	$$PWD/../terf/terf/terf.cpp \
    audio/audio-stream.cpp \
    audio/tremor-dec.cpp

SOURCES += \
	$$DEP_PATH/sqlite-amalgamation-3140100/sqlite3.c \

INCLUDE_DIR += $$PWD

INCLUDEPATH += \
	$$DEP_PATH/colorado \
	$$DEP_PATH/glee/include/GL \
	$$DEP_PATH/glm-0.9.6.1/glm \
	$$DEP_PATH/imgui \
	$$DEP_PATH/iqm \
	$$WIN_DEPS/LuaJIT-2.0.4/src \
	$$WIN_DEPS/openal-soft-1.17.2-bin/include \
	$$WIN_DEPS/SDL-1.2.15/include \
	$$WIN_DEPS/SDL-1.2.15/include/SDL \
	$$SOIL_PATH \
	$$DEP_PATH/sqlite-amalgamation-3140100 \
	$$DEP_PATH/terf \

DEPENDPATH += \
	$$DEP_PATH/colorado \
	$$DEP_PATH/terf \

CONFIG(debug, debug|release) {
	LIBS += \
		-L$$DEP_PATH/build-colorado-shared-windows-Desktop_Qt_5_7_0_MinGW_32bit-Debug/debug/ \
		-L$$DEP_PATH/colorado/build-video-encoder-null-Desktop_Qt_5_7_0_MinGW_32bit-Debug/debug/ \
		-L$$DEP_PATH/terf/build-terf-Desktop_Qt_5_7_0_MinGW_32bit-Debug/debug \
}
CONFIG(release, debug|release) {
	LIBS += \
		-L$$DEP_PATH/build-colorado-shared-windows-Desktop_Qt_5_7_0_MinGW_32bit-Release/release/ \
		-L$$DEP_PATH/colorado/build-video-encoder-null-Desktop_Qt_5_7_0_MinGW_32bit-Release/release/ \
		-L$$DEP_PATH/terf/build-terf-Desktop_Qt_5_7_0_MinGW_32bit-Release/release \
}

LIBS += \
	-L"$$DEP_PATH/build-glee-Desktop_Qt_5_7_0_MinGW_32bit-Default" -lglee -lOpenGL32 \
	-L"$$WIN_DEPS/LuaJIT-2.0.4/src" -llua51 \
	-L$$WIN_DEPS/openal-soft-1.17.2-bin/libs/Win32 -lOpenAL32 \
	-L"$$WIN_DEPS/SDL-1.2.15/lib" -L"$$WIN_DEPS/SDL-1.2.15/bin" -lmingw32 -lSDLmain -lSDL \
	-L$$DEP_PATH/terf/build-terf-Desktop_Qt_5_7_0_MinGW_32bit-Release/release/ \

LIBS += -lcolorado

HEADERS += \
    audio/audio-stream.h \
    audio/tremor-dec.h \
    audio/stb-vorbis-dec.h \
    audio/audio.h
