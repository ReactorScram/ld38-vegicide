local coloradoRoot = "/home/user/projects/colorado"

package.path = string.format ("%s;%s/scripts/?.lua;%s/lua/redist/?.lua", package.path,
	coloradoRoot, coloradoRoot)

if not arg [2] then
	print [[
Usage: 

lua vertex-checker.lua <smooth.lua> <flat.lua>

smooth.lua should have the sharp edges split
sharp.lua should have all edges split

Output: The sharp model's vertices and UVs with the normals of the smooth model
	]]
	os.exit ()
end

local lume = require "lume"
local exportModel = require "export-model"

local models = {
	loadfile (arg [1])(), 
	loadfile (arg [2])(),
}

--[[
Constructs an index for the mesh along one axis
dimension == 1: x
dimension == 2: y
dimension == 3: z

This is used to speed up vertex lookup later on

The index is a hash from a real-number position to a 
set of vert indices from 0 to n.
--]]

local function indexByDimension (verts, dimension)
	local index = {}
	
	for i, vert in ipairs (verts) do
		local x = vert [dimension]
		
		local indexRow = index [x] or {}
		
		indexRow [i] = true
		
		index [x] = indexRow
	end
	
	return index
end

local function indexMesh (mesh)
	local indices = {}
	for i = 1, 3 do
		indices [i] = indexByDimension (mesh.verts, i)
	end
	
	return indices
end

local function checkDuplicates (a, b)
	return a [1] == b [1] and a [2] == b [2] and a [3] == b [3]
end

local function findDuplicates (verts, indices, vert)
	local duplicates = {}
	
	for i, _ in pairs (indices [1][vert [1]]) do
		if indices [2][vert [2]][i] and indices [3][vert [3]][i] then
			-- This assert is working fine so I'm taking it out
			--assert (checkDuplicates (vert, verts [i]))
			table.insert (duplicates, i)
		end
	end
	
	return duplicates
end

local function dot (a, b)
	return a [1] * b [1] + a [2] * b [2] + a [3] * b [3]
end

local newModel = {
	meshes = {},
	objects = models [1].objects,
}

for i, _ in ipairs (models [1].meshes) do
	local meshes = lume.map (models, function (model) return model.meshes [i] end)

	-- The flat mesh has more vertices because every edge is split
	local flatMesh, smoothMesh
	if #meshes [1].verts < #meshes [2].verts then
		flatMesh = meshes [2]
		smoothMesh = meshes [1]
	else
		flatMesh = meshes [1]
		smoothMesh = meshes [2]
	end
	
	local flatIndex = indexMesh (flatMesh)
	local smoothIndex = indexMesh (smoothMesh)

	for i, vert in ipairs (flatMesh.verts) do
		local normal = flatMesh.normals [i]
		
		-- Find all the vertices at the exact same point on the smoothed mesh
		local smoothIndices = findDuplicates (smoothMesh.verts, smoothIndex, vert)
		
		-- Get their smooth normals
		local smoothNormals = lume.map (smoothIndices, function (i) return smoothMesh.normals [i] end)
		
		local dots = lume.map (smoothNormals, function (b)
			local a = normal
			return dot (a, b)
		end)
		
		local bestDot = dots [1]
		normal = smoothNormals [1]
		for i = 2, #smoothNormals do
			if dots [i] > bestDot then
				bestDot = dots [i]
				normal = smoothNormals [i]
			end
		end
		
		flatMesh.normals [i] = normal
	end
	
	newModel.meshes [i] = flatMesh
end

print (exportModel (newModel))
