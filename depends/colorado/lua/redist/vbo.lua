local libs = require "libcolorado"

local ffi = require "ffi"

local Buffer = libs.Buffer
local Colorado = libs.Colorado

local Gl = require "Colorado.glew-lua"
local checkGlError = require "Colorado.gl-error"
local TableWriter = require "Colorado.table-writer"
local lume = require "Colorado.lume"
local Mesh = require "Colorado.mesh"
local M = {
	indexFormat = Gl.UNSIGNED_SHORT,
}
M.__index = M

function M.new (fn)
	local bufferNames = ffi.new ("GLuint [?]", 2)
	
	Gl.GenBuffers (2, bufferNames)
	
	assert (bufferNames [0] > 0)
	assert (bufferNames [1] > 0)
	
	local self = {
		vbo = bufferNames [0],
		ibo = bufferNames [1],
		indexCount = 0,
		meshStrips = {},
		-- The maximum currently supported
		numTexCoords = 2,
	}
	setmetatable (self, M)
	
	if type (fn) == "string" then
		self:loadFromColoradoFile (fn)
	end
	
	return self
end

function M:__gc ()
	local bufferNames = ffi.new ("GLuint [2]", self.vbo, self.ibo)
	Gl.DeleteBuffers (2, bufferNames)
end

function M:allocateBuffers (vertexBytes, indexBytes)
	Gl.BufferData (Gl.ARRAY_BUFFER, vertexBytes, nil, Gl.STATIC_DRAW)
	Gl.BufferData (Gl.ELEMENT_ARRAY_BUFFER, indexBytes, nil, Gl.STATIC_DRAW)
end

function M:bindAndPrepare (shader)
	self:bindBuffers ()
	self:prepareVertexAttribs (shader)
end

function M:bindBuffers ()
	Gl.BindBuffer (Gl.ARRAY_BUFFER, self.vbo)
	Gl.BindBuffer (Gl.ELEMENT_ARRAY_BUFFER, self.ibo)
	--checkGlError "Bound VBO buffers"
end

--[[
function M:loadFromColoradoFile (fn)
	local source = Colorado.readPhysFsFileToBuffer (fn)
	if source ~= nil then
		local model = load (source) ()
		return self:loadFromColoradoTable (model)
	end
end
--]]
function M:loadFromColoradoTable (model)
	return self:loadFromMeshes (model.meshes)
end
--[[
function M:loadFromMesh (mesh)
	if mesh == nil then
		error "Vbo:loadFromMesh: mesh is nil"
	end
	
	local vertexTable = Mesh.convertVerticesToTable (mesh)
	local indexTable = Mesh.convertFacesToTable (mesh)
	
	self:bindBuffers ()
	
	local bytes = #vertexTable * 4
	local vertexBuffer = Buffer.new (bytes)
	
	Buffer.setFloats (vertexBuffer, 0, vertexTable)
	
	Gl.BufferData (Gl.ARRAY_BUFFER, bytes, nil, Gl.STATIC_DRAW)
	Gl.BufferSubData (Gl.ARRAY_BUFFER, 0, bytes, vertexBuffer)
	
	
	self.indexCount = #indexTable
	local indexBytes = self.indexCount * 4
	local indexBuffer = Buffer.new (indexBytes)
	
	Buffer.setInts (indexBuffer, 0, indexTable)
	
	Gl.BufferData (Gl.ELEMENT_ARRAY_BUFFER, indexBytes, nil, Gl.STATIC_DRAW)
	Gl.BufferSubData (Gl.ELEMENT_ARRAY_BUFFER, 0, indexBytes, indexBuffer)
	
	Buffer.delete (vertexBuffer)
	Buffer.delete (indexBuffer)
end
--]]

function M:loadFromMeshes (meshes)
	local tables = Mesh.loadToTables (meshes, self.numTexCoords)
	self:loadFromTables (tables)
end

function M:loadFromTables (tables)	
	local totalVertexBytes = tables.totalVertexBytes
	local totalIndexCount = tables.totalIndexCount
	local totalIndexBytes = tables.totalIndexBytes
	
	local objects = tables.objects
	
	local splitMeshes = tables.splitMeshes
	local meshStrips = tables.meshStrips
	
	self.meshStrips = meshStrips
	
	self.indexCount = totalIndexCount
	
	-- Allocate GPU memory
	self:bindBuffers ()
	
	assert (totalVertexBytes ~= 0)
	assert (totalIndexBytes ~= 0)
	
	Gl.BufferData (Gl.ARRAY_BUFFER, totalVertexBytes, nil, Gl.STATIC_DRAW)
	Gl.BufferData (Gl.ELEMENT_ARRAY_BUFFER, totalIndexBytes, nil, Gl.STATIC_DRAW)
	
	-- Load each mesh into GPU memory
	local vertexBytesOffset = 0
	local indexBytesOffset = 0
	
	local strips = {}
	local materialStrips = {}
	
	for i, object in ipairs (objects) do
		if object.vertexBuffer then
			Gl.BufferSubData (Gl.ARRAY_BUFFER, vertexBytesOffset, 
			object.vertexBytes, object.vertexBuffer)
		else
			local vertexBuffer = Buffer.new (object.vertexBytes)
			Buffer.setFloats (vertexBuffer, 0, object.vertexTable)
			Gl.BufferSubData (Gl.ARRAY_BUFFER, vertexBytesOffset, 
			object.vertexBytes, vertexBuffer)
			Buffer.delete (vertexBuffer)
		end
		
		if object.indexBuffer then
			Gl.BufferSubData (Gl.ELEMENT_ARRAY_BUFFER, indexBytesOffset, 
			object.indexBytes, object.indexBuffer)
		else
			local indexBuffer = Buffer.new (object.indexBytes)
			Buffer.setInts (indexBuffer, 0, object.indexTable)
			Gl.BufferSubData (Gl.ELEMENT_ARRAY_BUFFER, indexBytesOffset, 
			object.indexBytes, indexBuffer)	
			Buffer.delete (indexBuffer)
		end
		
		
		local mesh = splitMeshes [i]
		
		local bytesPerIndex = 2
		
		local strip = {
			offset = indexBytesOffset / bytesPerIndex,
			length = object.indexBytes / bytesPerIndex,
		}
		
		strips [mesh.name] = strip
		
		--print ("Mesh material count:", #(mesh.materials or {}))
		
		for i, t in ipairs (mesh.materials or {}) do
			local nextT = mesh.materials [i + 1] or {
				strip.length,
			}
			
			local materialStrip = {
				offset = strip.offset + t [1],
				length = nextT [1] - t [1],
			}
			
			local name = mesh.name
			
			local stripName = name .. "/" .. t [2]
			materialStrips [stripName] = materialStrip
			
			--print ("Strip", stripName, materialStrip.offset)
		end
		
		vertexBytesOffset = vertexBytesOffset + object.vertexBytes
		indexBytesOffset = indexBytesOffset + object.indexBytes
	end
	
	self.strips = strips
	self.materialStrips = materialStrips
	
	return strips
end

function M:prepareVertexAttribs (shader)
	local cast = function (x)
		return ffi.cast ("void *", x)
	end
	
	local attribPos = shader.attributes.attribPos
	local attribNormal = shader.attributes.attribNormal
	local attribTexCoords = {
		shader.attributes.attribTexCoord,
		shader.attributes.attribTexCoord1,
	}
	
	local floatSize = 4
	
	local numFloats = 0
	
	-- pos and normal
	numFloats = numFloats + 3
	numFloats = numFloats + 4
	
	for i = 1, self.numTexCoords do
		--print ("prepareVertexAttribs", i)
		numFloats = numFloats + 2
	end
	
	local stride = numFloats * floatSize
	
	--print ("prepareVertexAttribs", stride, numFloats)
	
	local bytes = 0
	
	local dim = 3
	if attribPos and attribPos >= 0 then
		Gl.VertexAttribPointer (attribPos, dim, Gl.FLOAT, false, stride, cast (bytes))
	end
	bytes = bytes + floatSize * dim
	
	local dim = 4
	if attribNormal and attribNormal >= 0 then
		Gl.VertexAttribPointer (attribNormal, dim, Gl.FLOAT, false, stride, cast (bytes))
	end
	bytes = bytes + floatSize * dim
	
	for i = 1, self.numTexCoords do
		local dim = 2
		local attribTexCoord = attribTexCoords [i]
		if attribTexCoord and attribTexCoord >= 0 then
			Gl.VertexAttribPointer (attribTexCoord, dim, Gl.FLOAT, false, stride, cast (bytes))
		end
		bytes = bytes + floatSize * dim
	end
	
	--checkGlError "Prepared VBO attrib pointers"
end

function M:render ()
	Gl.DrawElements (Gl.TRIANGLES, self.indexCount, self.indexFormat, nil)
end

function M:renderMesh (meshStrip)
	self:renderStrip (meshStrip.offset, meshStrip.length)
end

function M:renderStrip (offset, length)
	if length == nil then
		offset, length = offset.offset, offset.length
	end
	
	local bytesPerIndex = 2
	
	Gl.DrawElements (Gl.TRIANGLES, length, self.indexFormat, ffi.cast ("void *", offset * bytesPerIndex))
end

return M
