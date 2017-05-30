TARGET = colorado-audio-encoder
TEMPLATE = app
#CONFIG += shared
CONFIG -= qt
OBJECTS_DIR = ./obj
CONFIG += link_pkgconfig

# FFMpeg
LIBS += -lavcodec -lavformat -lavutil -lswscale

INC_DIR = $$PWD

INCLUDEPATH += ../

SOURCES += \
	$$PWD/main-audio.cpp
