local libs = require "libcolorado"
local ffi = require "ffi"

local PANDORA = libs.Colorado.COLORADO_PANDORA

require "glua.gl.gl3"
--[[
local Gl = ffi.load ((PANDORA and "GLESv2") or
	(ffi.os == "Windows" and "opengl32") or
	"GL", true)
--]]
local GLEW = ffi.load (
	(PANDORA and "GLESv2") or
	(ffi.os == "Windows" and "glew32") or
	"GLEW", true)

return GLEW