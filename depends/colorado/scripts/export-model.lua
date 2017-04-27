local lume = require "lume"

local function exportVectors (vectors)
	local normalStrings = lume.map (vectors, function (normal) 
		return string.format ("{%s},\n", table.concat (normal, ", "))
	end)
	return string.format (" = {\n%s},", table.concat (normalStrings))
end

return function (model)
	local t = {}
	local function ins (x)
		table.insert (t, x)
	end
	
	ins "local meshes = {\n"
		for _, mesh in ipairs (model.meshes) do
			ins "{\n"
			ins (string.format ('name = "%s",\n', mesh.name))
			ins "verts"
			ins (exportVectors (mesh.verts))
			ins "\nnormals"
			ins (exportVectors (mesh.normals))
			ins "\ntexCoords"
			if mesh.texCoords then
				ins (exportVectors (mesh.texCoords))
			else
				ins " = nil,"
			end
			ins "\nfaces"
			ins (exportVectors (mesh.faces))
			ins "},\n"
		end
	ins "}\n\n"
	
	ins "local objects = {\n"
		for _, object in ipairs (model.objects) do
			ins "{\n"
				ins (string.format ('name = "%s",\n', object.name))
				ins "matrix = {\n"
				-- What is a matrix but a long vector
				ins (table.concat (object.matrix, ", "))
				ins "\n},\n"
				ins "meshes = {\n"
				ins (table.concat (object.meshes, ", "))
				ins "\n},\n"
			ins "},\n"
		end
	ins "}\n\n"
	
	ins [[
		return {
			meshes = meshes,
			objects = objects,
		}
	]]
	
	return table.concat (t)
end
