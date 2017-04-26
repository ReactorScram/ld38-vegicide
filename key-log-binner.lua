-- Usage: luajit key-log-binner.lua < key_log.txt > key_log.bin

local ffi = require 'ffi'

local function write_8 (n)
	io.write (ffi.string (ffi.typeof ("uint8_t[1]")(n), 1))
end

local function write_64 (n)
	io.write (ffi.string (ffi.typeof ("uint64_t[1]")(n), 8))
end

for line in io.lines () do
	if line:sub (1, 1) == "#" then
		-- Comment, pass
	else
		frames, down, code = line:match ("(%d+), (%d+), (%d+)")
		
		local to_n = tonumber
		
		frames = to_n (frames)
		down = to_n (down)
		code = to_n (code)
		
		write_64 (frames)
		write_8 (down)
		write_8 (code)
	end
end
