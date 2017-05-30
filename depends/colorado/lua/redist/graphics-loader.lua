local ffi = require "ffi"

local libs = require "libcolorado"
local Colorado = libs.Colorado
local pandora = Colorado.COLORADO_PANDORA

local Object = require "Colorado.classic"
local lume = require "Colorado.lume"

local terf = require "terfArchive"
local Gl = require "Colorado.glew-lua"
local Shader = require "Colorado.shader"
local Vbo = require "Colorado.vbo"

local M = Object:extend ()

function M.loadShader (name)
	local vertFn = string.format ("shaders/%s.vert", name)
	local fragFn = string.format ("shaders/%s.frag", name)
	
	local shader = Shader.new ()
	
	local success, err, log = shader:load (terf, vertFn, fragFn)
	if success then
		local program = Colorado.Shader.program (shader.program)
		
		-- Get attributes
		do
			local activeAttributes = ffi.new ("int [1]")
			Gl.GetProgramiv (program, Gl.ACTIVE_ATTRIBUTES, activeAttributes)
			activeAttributes = activeAttributes [0]
			
			local attributeMaxLength = ffi.new ("int [1]")
			Gl.GetProgramiv (program, Gl.ACTIVE_ATTRIBUTE_MAX_LENGTH, attributeMaxLength)
			attributeMaxLength = attributeMaxLength [0]
			
			local attributeNameBuffer = ffi.new ("char [?]", attributeMaxLength)
			
			for i = 0, activeAttributes - 1 do
				Gl.GetActiveAttrib (program, i, attributeMaxLength, nil, ffi.new ("int [1]"), ffi.new ("unsigned int [1]"), attributeNameBuffer)
				
				local attributeName = ffi.string (attributeNameBuffer)
				
				shader:locateAttribute (attributeName)
			end
		end
		
		-- Get uniforms
		do
			local activeUniforms = ffi.new ("int [1]")
			Gl.GetProgramiv (program, Gl.ACTIVE_UNIFORMS, activeUniforms)
			activeUniforms = activeUniforms [0]
			
			local uniformMaxLength = ffi.new ("int [1]")
			Gl.GetProgramiv (program, Gl.ACTIVE_UNIFORM_MAX_LENGTH, uniformMaxLength)
			uniformMaxLength = uniformMaxLength [0]
			
			local uniformNameBuffer = ffi.new ("char [?]", uniformMaxLength)
			
			for i = 0, activeUniforms - 1 do
				Gl.GetActiveUniform (program, i, uniformMaxLength, nil, ffi.new ("int [1]"), ffi.new ("unsigned int [1]"), uniformNameBuffer)
				
				local uniformName = ffi.string (uniformNameBuffer)
				
				shader:locateUniform (uniformName)
			end
			
			for i, v in pairs (shader.uniforms) do
				--print ("Uniform", i, v)
			end
		end
	else
		print ("Shader error:", name, err)
		print (log)
	end
	
	return shader
end

function M.loadShaders (shadersToLoad)
	local shaders = {}
	
	for name, fileName in pairs (shadersToLoad) do
		shaders [name] = M.loadShader (fileName)
	end
	
	return shaders
end

function M.loadTexture (fn)
	local pandora = Colorado.COLORADO_PANDORA
	
	if pandora then
		fn = string.format ("images-pvr/%s.pvr", fn)
	else
		fn = string.format ("images/%s", fn)
	end
	
	local buffer = terf:lookupFile (fn)
	assert (buffer, fn)
	assert (#buffer > 0, fn)
	
	local texture = nil
	if pandora then
		texture = Colorado.loadCompressedTextureFromBuffer (buffer)
	else
		texture = Colorado.loadTextureFromBuffer (buffer)
	end
	
	if type (texture) == "number" then
		Gl.BindTexture (Gl.TEXTURE_2D, texture)
		if pandora then
			Gl.TexParameteri (Gl.TEXTURE_2D, Gl.TEXTURE_MIN_FILTER,
				Gl.NEAREST_MIPMAP_LINEAR)
			Gl.TexParameteri (Gl.TEXTURE_2D, Gl.TEXTURE_MAG_FILTER,
				Gl.LINEAR)
		else
			Gl.TexParameteri (Gl.TEXTURE_2D, Gl.TEXTURE_MIN_FILTER,
				Gl.NEAREST_MIPMAP_LINEAR)
			Gl.TexParameteri (Gl.TEXTURE_2D, Gl.TEXTURE_MAG_FILTER,
				Gl.LINEAR)
		end
	end
	
	return texture
end

-- TODO: Move into Colorado
function M.loadTextures (textureFiles)
	local textures = {}
	
	for code, fn in pairs (textureFiles) do
		textures [code] = M.loadTexture (fn)
	end
	
	return textures
end

function M.loadModel (shortFileName)
	local fn = string.format ("meshes/%s.lua", shortFileName)
	local model = load (terf:lookupFile (fn))()
	return model
end

function M.loadModels (nameList)
	return lume.map (nameList, function (shortFileName)
		return M.loadModel (shortFileName)
	end)
end

function M.loadVbos (models)
	return lume.map (models, function (model)
		return M.loadVbo (model)
	end)
end

function M.loadVbo (model)
	local result = Vbo.new ()
	local strips = result:loadFromColoradoTable (model)
	
	return result, strips
end

return M
