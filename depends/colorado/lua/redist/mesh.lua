-- All the parts of Vbo that don't depend on TERF, GL, etc.
-- Pretty much static functions
local M = {}

local ffi = require "ffi"
local TableWriter = require "Colorado.table-writer"
local lume = require "Colorado.lume"

function M.convertFacesToTable (mesh, offset)
	offset = offset or 0
	
	local indexTable = {}
	local i = 1
	local function addIndex (index)
		indexTable [i] = index + offset
		i = i + 1
	end
	for _, face in ipairs (mesh.faces) do
		for i = 3, #face do
			addIndex (face [1])
			addIndex (face [i - 1])
			addIndex (face [i])
		end
	end
	
	return indexTable
end

-- Converts the Colorado format to an interleaved VBO format
function M.convertVerticesToTable (mesh, matrixIndex, numTexCoords)
	-- This function was giving me some trouble in Phone Fighter
	jit.off (true)
	
	matrixIndex = matrixIndex or 0
	
	local vertexTable, addV = TableWriter.makeTableAndWriter ()
	local i
	for i = 1, #mesh.verts do
		local pos = mesh.verts [i]
		addV (pos [1])
		addV (pos [2])
		addV (pos [3])
		
		local normal = mesh.normals [i]
		addV (normal [1])
		addV (normal [2])
		addV (normal [3])
		-- Normal W, currently unused
		addV (0.0)
		
		-- TODO: Texture coords
		if mesh.texCoords ~= nil then
			for j = 1, numTexCoords do
				local texCoords = mesh.texCoords [j]
				
				if texCoords then
					local texCoord = texCoords [i]
					if texCoord == nil then
						error (string.format ("Texcoord %i, %i is nil", j, i))
					end
					
					addV (texCoord [1])
					addV (texCoord [2])
				else
					addV (0.0)
					addV (0.0)
				end
			end
		else
			for j = 1, numTexCoords do
				addV (0.0)
				addV (0.0)
			end
		end
	end
	
	return vertexTable
end

-- Splits a mesh into individual triangles to allow UV seams to work properly
function M.splitMesh (mesh)
	if mesh.faceUvCoords and not mesh.faceUvCoordArray then
		mesh.faceUvCoordArray = { 
			{
				name = "",
				data = mesh.faceUvCoords,
			}
		}
	end
	
	if mesh.faceUvCoordArray == nil then
		--[[
		We make a shallow copy here because otherwise we would be mutating the 
		mesh itself and making it impossible to load the same mesh multiple 
		times - which is precisely what Byte Raider does.
		--]]
		local shallowCopy = {}
		for i, v in pairs (mesh) do
			shallowCopy [i] = v
		end
		
		shallowCopy.texCoords = { mesh.texCoords }
		return shallowCopy
	end
	
	local faces = {}
	
	local verts = {}
	local normals = {}
	local texCoords = {}
	
	for i, _ in ipairs (mesh.faceUvCoordArray) do
		texCoords [i] = {}
	end
	
	local outputIndex = 1
	local function addVert (index, uvCoords)
		verts [outputIndex] = mesh.verts [index]
		normals [outputIndex] = mesh.normals [index]
		
		for i, uvCoord in ipairs (uvCoords) do
			texCoords [i][outputIndex] = uvCoord
		end
	end
	
	for faceIndex, face in ipairs (mesh.faces) do
		local outputFace = {}
		for faceVertIndex, index in ipairs (face) do
			local uvCoords = {}
			
			for i, uvMap in ipairs (mesh.faceUvCoordArray) do
				uvCoords [i] = uvMap.data [faceIndex] [faceVertIndex]
			end
			
			addVert (index + 1, uvCoords)
			outputFace [faceVertIndex] = outputIndex - 1
			outputIndex = outputIndex + 1
		end
		
		faces [faceIndex] = outputFace
	end
	
	return {
		verts = verts,
		normals = normals,
		texCoords = texCoords,
		faces = faces,
		materials = mesh.materials,
		name = mesh.name,
	}
end

function M.loadToTables (meshes, numTexCoords)
	local totalVertexCount = 0
	local totalVertexComponentCount = 0
	local totalVertexBytes = 0
	local totalIndexCount = 0
	local totalIndexBytes = 0
	
	local bytesPerVertexComponent = 4
	local bytesPerIndex = 2
	
	local objects = {}
	
	local splitMeshes = {}
	local meshStrips = {}
	
	-- Load each mesh into a CPU-side buffer in the GPU-ready format
	-- While doing this, tally up the bytes and indices used
	for i, mesh in ipairs (meshes) do
		local splitMesh = M.splitMesh (mesh)
		splitMeshes [i] = splitMesh
		
		local vertexTable = M.convertVerticesToTable (splitMesh, 0, numTexCoords)
		local indexTable = M.convertFacesToTable (splitMesh, totalVertexCount)
		
		local vertexCount = #splitMesh.verts
		local vertexComponentCount = #vertexTable
		local vertexBytes = vertexComponentCount * bytesPerVertexComponent
		
		local indexCount = #indexTable
		local indexBytes = indexCount * bytesPerIndex
		
		assert (vertexBytes > 0)
		assert (indexBytes > 0)
		--print ("bytes", vertexBytes, indexBytes)
		
		local object = {
			vertexTable = vertexTable,
			indexTable = indexTable,
			vertexBytes = vertexBytes,
			vertexCount = vertexCount,
			indexBytes = indexBytes,
			indexCount = indexCount,
		}
		
		table.insert (objects, object)
		
		meshStrips [i] = {
			offset = totalIndexCount,
			length = indexCount,
		}
		
		totalVertexBytes = totalVertexBytes + vertexBytes
		totalVertexCount = totalVertexCount + vertexCount
		totalVertexComponentCount = totalVertexComponentCount + vertexComponentCount
		totalIndexBytes = totalIndexBytes + indexBytes
		totalIndexCount = totalIndexCount + indexCount
	end
	
	return {
		totalVertexBytes = totalVertexBytes,
		totalIndexBytes = totalIndexBytes,
		totalIndexCount = totalIndexCount,
		
		objects = objects,
		splitMeshes = splitMeshes,
		meshStrips = meshStrips,
	}
end

function M.sequenceToString (sequence, type)
	local buffer = ffi.new (type .. " [?]", #sequence)
	
	for i = 0, #sequence - 1 do
		buffer [i] = sequence [i + 1]
	end
	
	return ffi.string (buffer, #sequence * ffi.sizeof (type))
end

function M.stringToSequence (s, type)
	local count = #s / ffi.sizeof (type)
	local t = {}
	local buffer = ffi.new (type .. " [?]", count + 1)
	ffi.copy (buffer, s)
	for i = 1, count do
		t [i] = buffer [i - 1]
	end
	return t
end

function M.encodePackedBuffers (modelTable)
	for k, v in pairs (modelTable.splitMeshes) do
		v.verts = nil
		v.normals = nil
		v.texCoords = nil
		v.faces = nil
	end
	
	for k, v in ipairs (modelTable.objects) do
		v.vertexBuffer = M.sequenceToString (v.vertexTable, "float")
		v.vertexTable = nil
		
		v.indexBuffer = M.sequenceToString (v.indexTable, "uint16_t")
		v.indexTable = nil
	end
end

-- Part of the BSON loading process
function M.decodePackedBuffers (modelTable) 
    for k, v in pairs (modelTable.objects) do
        v.vertexTable = M.stringToSequence (v.vertexBuffer, "float")
        v.indexTable = M.stringToSequence (v.indexBuffer, "uint16_t")
    end
end

return M
