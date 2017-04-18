#### PROJECT SETTINGS ####
# The name of the executable to be created
BIN_NAME := libcolorado_scene.so
# Space-separated pkg-config libraries used by this project
LIBS = openal opusfile luajit sdl sqlite3
# Add additional include paths
INCLUDES = -I $(SRC_PATH) -I .. -I $(DEP_PATH)/colorado -I $(DEP_PATH)/terf -I $(DEP_PATH)/iqm -I /usr/include/SOIL
# General linker settings
LINK_FLAGS = -L ../audio/ -L ../graphics/ -lGLee -lGL -lopusfile -lcolorado -lterf -lSOIL -fPIC -shared
# Additional release-specific linker settings
RLINK_FLAGS = -L $(DEP_PATH)/terf/build-terf-Desktop-Release -L $(DEP_PATH)/build-colorado-shared-arch-Desktop-Release -L $(DEP_PATH)/colorado/build-video-encoder-null-Desktop-Release
# Additional debug-specific linker settings
DLINK_FLAGS = -L $(DEP_PATH)/terf/build-terf-Desktop-Debug -L$(DEP_PATH)/build-colorado-shared-arch-Desktop-Debug -L $(DEP_PATH)/colorado/build-video-encoder-null-Desktop-Debug
#### END PROJECT SETTINGS ####

include ../Makefile.common
