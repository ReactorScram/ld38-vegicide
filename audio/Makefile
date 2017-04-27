#### PROJECT SETTINGS ####
# The name of the executable to be created
BIN_NAME := libColoradoAudio.so
# Space-separated pkg-config libraries used by this project
LIBS = openal
# Add additional include paths
INCLUDES = -I $(SRC_PATH) -I $(DEP_PATH)/colorado -I $(DEP_PATH)/terf
# General linker settings
LINK_FLAGS = -fPIC -shared
# Additional release-specific linker settings
RLINK_FLAGS =
# Additional debug-specific linker settings
DLINK_FLAGS =
#### END PROJECT SETTINGS ####

include ../Makefile.common
