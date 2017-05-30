cmake_minimum_required (VERSION 2.6)
project (Colorado)

option (USE_PANDORA "Compile for OpenPandora" OFF)
option (USE_X264 "x264 video recorder" OFF)

include_directories (".")
include_directories ("../glm-0.9.4.6/glm")

include (FindPkgConfig)

# PhysFS
set (LINK_LIBRARIES ${LINK_LIBRARIES} physfs)

# SDL
pkg_check_modules (SDL REQUIRED sdl)
include_directories (${SDL_INCLUDE_DIRS})
set (LINK_LIBRARIES ${LINK_LIBRARIES} ${SDL_LIBRARIES})



# SOIL
set (SOIL_SOURCE_DIR "../soil/src")
include_directories (${SOIL_SOURCE_DIR})
set (SOURCE ${SOURCE}
"${SOIL_SOURCE_DIR}/SOIL.c"
"${SOIL_SOURCE_DIR}/image_DXT.c"
"${SOIL_SOURCE_DIR}/image_helper.c"
"${SOIL_SOURCE_DIR}/stb_image_aug.c"
)

set (SOURCE ${SOURCE}
game.cpp
game-lua.cpp
gl.cpp
gl-buffer.cpp
gl-shader.cpp
lua-5.1-helper.cpp
lua-state.cpp
lua/colorado.cpp
lua/input.cpp
lua/lua-buffer.cpp
lua/lua-game.cpp
lua/lua-gl.cpp
lua/lua-matrix.cpp
lua/lua-physfs.cpp
lua/lua-sdl.cpp
lua/lua-ui-vbo-node.cpp
lua/matrix.cpp
lua/shader.cpp
read-file-physfs.cpp
video-encoder-c.cpp
)

#add_definitions (-DCOLORADO_USE_LUAJIT)

if (USE_PANDORA)
	set (EGLPORT_ROOT "../eglport")
	set (SOURCE ${SOURCE} 
	"${EGLPORT_ROOT}/eglport.c"
	)
	include_directories (${EGLPORT_ROOT})
	add_definitions (-DCOLORADO_HAVE_GLES -DUSE_EGL_SDL -DUSE_GLES2 
	-DCOLORADO_PANDORA)
	set (LINK_LIBRARIES ${LINK_LIBRARIES} "EGL" "GLESv2")
	
	# Lua
	
	# PhysFS
	include_directories ("../physfs-2.0.3/")
else (USE_PANDORA)
	# LuaJIT
	pkg_check_modules (LUAJIT REQUIRED luajit)
	include_directories (${LUAJIT_INCLUDE_DIRS})
	
	set (LINK_LIBRARIES ${LINK_LIBRARIES}
	GLEW
	)
	
	add_definitions (-DUSE_GLEW -DUSE_DESKTOP_GL)
endif (USE_PANDORA)

if (USE_X264)
	set (SOURCE ${SOURCE} video-encoder.cpp)
else (USE_X264)
	set (SOURCE ${SOURCE} video-encoder-null.cpp)
endif (USE_X264)

add_library (colorado SHARED ${SOURCE})
target_link_libraries (colorado ${LINK_LIBRARIES})
