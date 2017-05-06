-- lua dump-csv.lua file.bin > file.csv

local f = io.open (arg [1], "rb")

local buffer = f:read "*all"

f:close ()

for i = 1, #buffer do
	io.write (buffer.byte (i) .. ", ")
end
