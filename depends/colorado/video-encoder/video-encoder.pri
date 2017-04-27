TARGET = colorado-video-encoder
TEMPLATE = lib
CONFIG += shared
CONFIG -= qt
OBJECTS_DIR = ./obj
CONFIG += link_pkgconfig

# No EglPort because the Pandora isn't strong enough to encode video

# GLEW
win32 {
	INCLUDEPATH += "../glew-1.11.0/include"
	LIBS += -L"../glew-1.11.0/lib/" -lglew32
}
else {
	LIBS += -lGLEW
}

# SOIL
INCLUDEPATH += "../../soil/src"
SOURCES += \
	../../soil/src/SOIL.c \
	../../soil/src/image_DXT.c \
	../../soil/src/image_helper.c \
	../../soil/src/stb_image_aug.c

win32 {
	LIBS += -lOpenGL32
}

# x264 / FFMpeg
LIBS += -lx264
LIBS += -lavcodec -lavformat -lavutil -lswscale

INC_DIR = $$PWD

INCLUDEPATH += ../

HEADERS += \
	$$INC_DIR/video-encoder.h \
	$$INC_DIR/video-encoder-c.h \

SOURCES += $$PWD/video-encoder-c.cpp

SOURCES += \
	$$PWD/video-encoder.cpp
