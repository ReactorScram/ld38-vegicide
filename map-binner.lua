-- Usage: luajit map-binner.lua maps/farm.lua > maps/farm.bin

local ffi = require 'ffi'

local map_fn = arg [1]

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
		write_16 (v)
	end
end

local layer = map.layers [1]

write_layer (layer)
