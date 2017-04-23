-- Usage: luajit map-binner.lua --tiles maps/farm.lua > maps/farm.bin
-- Usage: luajit map-binner.lua --objects maps/farm.lua | sqlite3 maps/farm.sqlite

local ffi = require 'ffi'

local command = arg [1]
local map_fn = arg [2]

if not command or not map_fn then
	error ("Incorrect args")
end

local map = dofile (map_fn)

local function write_16 (n)
	io.write (ffi.string (ffi.typeof ("uint16_t[1]")(n), 2))
end

local function write_layer (layer)
	local width = layer.width
	local height = layer.height
	local data = layer.data

	assert (layer.encoding == "lua")

	write_16 (width)
	write_16 (height)
	
	for i, v in ipairs (data) do
		write_16 (v - 1)
	end
end

local function write_objects (layer)
	print "begin transaction;"
	print "drop table if exists vegicide_objects;"
	
	print "create table if not exists vegicide_objects (id integer primary key, name string, type string, x integer, y integer, width integer, height integer);"
	
	for _, object in ipairs (layer.objects) do
		print (string.format ("insert into vegicide_objects (id, name, type, x, y, width, height) values (%i, '%s', '%s', %i, %i, %i, %i);", object.id, object.name, object.type, math.floor (object.x), math.floor (object.y), math.floor (object.width), math.floor (object.height)))
	end
	
	print "commit;"
end

if command == "--tiles" then
	for _, layer in ipairs (map.layers) do
		if layer.type == "tilelayer" then
			write_layer (layer)
			break
		end
	end
elseif command == "--objects" then
	for _, layer in ipairs (map.layers) do
		if layer.type == "objectgroup" then
			write_objects (layer)
			break
		end
	end
end
