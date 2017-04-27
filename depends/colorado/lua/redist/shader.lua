local libs = libs or require "libcolorado"

local Colorado = libs.Colorado

local terf = require "terfArchive"
local Gl = require "Colorado.glew-lua"

local M = {}
M.__index = M

function M.new (vertFn, fragFn)
	local self = {
		attributes = {},
		program = nil,
		uniforms = {},
	}
	setmetatable (self, M)
	
	if vertFn ~= nil and fragFn ~= nil then
		self:load (terf, vertFn, fragFn)
	end
	
	return self
end

function M:__gc ()
	if self.program ~= nil then
		Colorado.Shader.delete (self.program)
	end
end

function M:bind ()
	Colorado.Shader.bind (self.program)
end

function M:load (terf, vertFn, fragFn)
	self.program = Colorado.Shader.new ()
	
	local result = Colorado.Shader.loadFromBuffers (self.program, terf:lookupFile (vertFn), terf:lookupFile (fragFn))
	
	if result.linkSuccess then
		-- Everything fine
	else
		return false, result.infoLog
	end
	
	local attribs = {
		"attribPos",
		"attribNormal",
		"attribTexCoord",
	}
	
	for _, name in ipairs (attribs) do
		self.attributes [name] = -1
	end
	
	return true
end

function M:locateAttribute (name)
	local location = Colorado.Shader.attributeLocation (self.program, name)
	self.attributes [name] = location
	return location
end

function M:locateUniform (name)
	local location = Colorado.Shader.uniformLocation (self.program, name)
	self.uniforms [name] = location
	return location
end

return M
