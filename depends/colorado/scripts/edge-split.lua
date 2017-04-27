local coloradoRoot = "/home/user/projects/colorado"

package.path = string.format ("%s;%s/scripts/?.lua;%s/lua/redist/?.lua", package.path,
	coloradoRoot, coloradoRoot)

if not arg [1] then
	print [[
Usage:

lua edge-split.lua <mesh.lua>

Takes a regular Lua mesh from lua_export.py, splits up all faces,
and applies UV coords.
	]]
	os.exit ()
end

local lume = require "lume"
local exportModel = require "export-model"

local function edgeSplitMesh (mesh)
	if not mesh.faceUvCoords and mesh.faceUvCoordArray then
		mesh.faceUvCoords = mesh.faceUvCoordArray [1]
	end
	
	if not mesh.faceUvCoords then
		return mesh
	end
	
	local dVerts = {}
	local dNormals = {}
	local dTexCoords = {}
	local dFaces = {}
	
	for sFaceIndex, sFace in ipairs (mesh.faces) do
		local dVertIndexBase = #dVerts
		
		local dFace = {}
		for i, sVertIndex in ipairs (sFace) do
			table.insert (dVerts, mesh.verts [sVertIndex + 1])
			table.insert (dNormals, mesh.normals [sVertIndex + 1])
			table.insert (dTexCoords, mesh.faceUvCoords [sFaceIndex][i])
			
			dFace [i] = dVertIndexBase + i - 1
		end
		
		table.insert (dFaces, dFace)
	end
	
	return {
		verts = dVerts,
		normals = dNormals,
		texCoords = dTexCoords,
		faces = dFaces,
		materials = mesh.materials,
		name = mesh.name,
	}
end

local model = loadfile (arg [1])()

local newModel = {
	meshes = {},
	objects = model.objects,
}

for i, mesh in ipairs (model.meshes) do
	newModel.meshes [i] = edgeSplitMesh (mesh)
end

print (exportModel (newModel))
