local Object = require "Colorado.classic"
local M = Object:extend ()

local checkGlError = require "Colorado.gl-error"

local Gl = require "Colorado.glew-lua"
local AttributeEnabler = require "Colorado.attribute-enabler"
function M:new (resources)
	self.attributeEnabler = AttributeEnabler.new ()
	
	self.shaders = resources.shaders
	self.vbos = resources.vbos
end

function M:bindShader (name)
	self.currentShader = self.shaders [name]
	self.currentShader:bind ()
	--checkGlError "Bound shader"
	self.attributeEnabler:enableAttributes (self.currentShader.attributes)
	--checkGlError "Enabled attributes"
end

function M:bindVbo (name)
	local newVbo = self.vbos [name]
	
	if self.currentVbo ~= newVbo then
		self.currentVbo = newVbo
		self.currentVbo:bindAndPrepare (self.currentShader)
		--checkGlError "Bound new VBO"
	end
end

function M:drawVbo (name, stripName)
	if name then
		self:bindVbo (name)
	end
	
	if stripName then
		self.currentVbo:renderStrip (self.currentVbo.strips [stripName])
	else
		self.currentVbo:render ()
	end
	--checkGlError "Rendered VBO"
end

function M:setMatrix (name, matrix)
	-- For some reason the GLua matrices are transposed?
	local uniformLocation = self:uniform (name)
	if uniformLocation >= 0 then
		Gl.UniformMatrix4fv (uniformLocation, 1, false, matrix.gl)
	end
end

function M:uniformVec3 (name, v)
	local uniformLocation = self:uniform (name)
	if uniformLocation >= 0 then
		Gl.Uniform3f (uniformLocation, v.x, v.y, v.z)
	end
end

function M:uniform (name)
	return self.currentShader.uniforms [name]
end

return M