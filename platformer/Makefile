#### PROJECT SETTINGS ####
# The name of the executable to be created
BIN_NAME := Platformer.exe
# Space-separated pkg-config libraries used by this project
LIBS = openal sdl sqlite3
# Add additional include paths
INCLUDES = -I $(SRC_PATH) -I .. -I $(DEP_PATH)/colorado -I $(DEP_PATH)/terf -I $(DEP_PATH) -I /usr/include/SOIL
# General linker settings
LINK_FLAGS = -L ../audio/ -L ../graphics/ -L ../depends \
-lGLESv2 -lcolorado_scene -lColoradoAudio -l ColoradoExcelsior -lEGL \
-lceecrap -leglport
# Additional release-specific linker settings
RLINK_FLAGS =
# Additional debug-specific linker settings
DLINK_FLAGS =
#### END PROJECT SETTINGS ####

include ../Makefile.common
