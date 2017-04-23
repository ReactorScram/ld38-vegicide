#### PROJECT SETTINGS ####
# The name of the executable to be created
BIN_NAME := libColoradoAudio.so
# Space-separated pkg-config libraries used by this project
LIBS = openal opusfile
# Add additional include paths
INCLUDES = -I $(SRC_PATH) -I $(DEP_PATH)/colorado -I $(DEP_PATH)/terf -I /usr/include/tremor 
# General linker settings
LINK_FLAGS = -lterf -lvorbisidec -fPIC -shared
# Additional release-specific linker settings
RLINK_FLAGS = -L $(DEP_PATH)/terf/build-terf-Desktop-Release
# Additional debug-specific linker settings
DLINK_FLAGS = -L $(DEP_PATH)/terf/build-terf-Desktop-Debug
#### END PROJECT SETTINGS ####

include ../Makefile.common
