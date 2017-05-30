local libs = libs or require "libcolorado"

local checkGlError = require "Colorado.gl-error"

local Gl = require "Colorado.glew-lua"
local M = {}
M.__index = M

function M.new ()
	local self = {
		-- A set of attributes. e.g. enabledAttribs.attribPos = true
		enabledAttribs = {},
	}
	setmetatable (self, M)
	
	return self
end

-- Call 
function M:clear ()
	
end

--[[ Takes a table of attributes such as
{
	attribPos = 0,
	attribNormal = 1,
}
in the same format as the shader class stores them.
--]]
function M:enableAttributes (attributes)
	local newAttribs = {}
	for _, i in pairs (attributes) do
		if i >= 0 then
			newAttribs [i] = true
			
			if not self.enabledAttribs [i] then
				Gl.EnableVertexAttribArray (i)
				checkGlError ("Enabled vertex attrib array", i)
			end
		end
	end
	
	for i, _ in pairs (self.enabledAttribs) do
		if i >= 0 and not newAttribs [i] then
			Gl.DisableVertexAttribArray (i)
			checkGlError ("Disabled vertex attrib array", i)
		end
	end
	
	self.enabledAttribs = newAttribs
end

return M