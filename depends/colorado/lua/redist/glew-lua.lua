local ffi = require "ffi"
local libs = require "libcolorado"

local PANDORA = libs.Colorado.COLORADO_PANDORA
local Sdl = ffi.load (PANDORA and "/usr/lib/libSDL-1.2.so.0" or "SDL")

ffi.cdef "void * SDL_GL_GetProcAddress (const char* proc);"

local terf = require "terfArchive"
-- Memoize GL functions
local memo = {}

-- Regular GL 1 stuff
local GLua = require "glua.init"

local fileName = "cdefs/glee2.cdef"
local source = terf:lookupFile (fileName)
ffi.cdef (source)

local M = {}

setmetatable (M, {
	__index = function (t, key)
		local glewName = "gl" .. key
		local func = memo [key]
		
		if func == nil then
			pcall (function ()
				func = GLua [key]
			end)
		end
		
		if func == nil then
			local rc, err = pcall (function ()
				local fp = Sdl.SDL_GL_GetProcAddress (glewName)
				
				if fp then
					-- It's okay to put GLEE here, we're only using the header
					local fpType = string.format ("GLEEPFN%sPROC", glewName:upper ())
					--local fpType = glewName
					func = ffi.cast (fpType, fp)
				end
			end)
			
			if not rc then
				print (err)
			end
		end
		
		if func == nil then
			error ("Could not find " .. key .. " anywhere")
		end
		
		memo [key] = func
		
		return func
	end,
})

return M
