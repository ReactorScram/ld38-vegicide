TARGET = colorado-video-encoder
TEMPLATE = lib
CONFIG += shared
CONFIG -= qt
OBJECTS_DIR = ./obj
CONFIG += link_pkgconfig

INC_DIR = $$PWD

INCLUDEPATH += ../

HEADERS += \
	$$INC_DIR/video-encoder.h \
	$$INC_DIR/video-encoder-c.h \

SOURCES += $$PWD/video-encoder-c.cpp

SOURCES += \
	$$PWD/video-encoder-null.cpp
