local ffi = require "ffi"
local Object = require "Colorado.classic"
local M = Object:extend ()

local Gl = require "Colorado.glew-lua"
local lume = require "Colorado.lume"

local bakedChars = require "baked-chars"

M.VBO = 0
M.IBO = 1
M.floatsPerVertex = 4
M.sizeOfFloat = ffi.sizeof ("float")
M.sizeOfInt = ffi.sizeof ("unsigned int")
M.verticesPerChar = 4
M.indicesPerChar = 6

function M:new ()
	self.bufferNames = ffi.new ("unsigned int [2]")
	
	Gl.GenBuffers (2, self.bufferNames)
	assert (self.bufferNames [self.VBO] ~= 0)
	assert (self.bufferNames [self.IBO] ~= 0)
end

function M:__gc ()
	Gl.DeleteBuffers (2, self.bufferNames)
	
	print "If this doesn't show up in the output somewhere, something bad happened"
end

--[[
Usage:

layoutText {
	{
		text = "Colorado Game Library",
		x = 0, 
		y = 24,
	},
	{
		text = "FPS: 60",
		x = 0,
		y = 48,
	},
}

Unicode probably doesn't work at all

Must be bound before calling layout ()
--]]

function M:layout (layout)
	local numChars = lume.reduce (lume.map (layout, function (x)
		return #x.text
	end), 
	function (a, b) 
		return a + b 
	end)
	
	local numVertexFloats = numChars * self.verticesPerChar * self.floatsPerVertex
	local numIndices = numChars * self.indicesPerChar
	
	local vertexArray = ffi.new ("float [?]", numVertexFloats)
	local indexArray = ffi.new ("unsigned int [?]", numIndices)
	
	-- The font starts at space which is the first printable Latin-1 char
	local baseCharByte = (" "):byte ()
	
	local indices = {
		0, 2, 1,
		0, 3, 2
	}
	
	-- Measured in characters
	local bufferCursor = 0
	for _, block in ipairs (layout) do
		local baseX = block.x
		local baseY = block.y
		
		for i = 1, #block.text do
			local char = block.text:sub (i, i)
			local charIndex = char:byte () - baseCharByte + 1
			
			local bakedChar = bakedChars [charIndex]
			local width = bakedChar.x1 - bakedChar.x0
			local height = bakedChar.y1 - bakedChar.y0
			
			local x = baseX + math.ceil (bakedChar.xoff)
			local y = baseY + math.ceil (bakedChar.yoff)
			
			local vi = bufferCursor * self.verticesPerChar * self.floatsPerVertex
			vertexArray [vi + 0] = x
			vertexArray [vi + 1] = y
			vertexArray [vi + 2] = bakedChar.x0 / 256.0
			vertexArray [vi + 3] = 1.0 - bakedChar.y0 / 256.0
			
			vertexArray [vi + 4] = x + width
			vertexArray [vi + 5] = y
			vertexArray [vi + 6] = bakedChar.x1 / 256.0
			vertexArray [vi + 7] = 1.0 - bakedChar.y0 / 256.0
			
			vertexArray [vi + 8] = x + width
			vertexArray [vi + 9] = y + height
			vertexArray [vi + 10] = bakedChar.x1 / 256.0
			vertexArray [vi + 11] = 1.0 - bakedChar.y1 / 256.0
			
			vertexArray [vi + 12] = x
			vertexArray [vi + 13] = y + height
			vertexArray [vi + 14] = bakedChar.x0 / 256.0
			vertexArray [vi + 15] = 1.0 - bakedChar.y1 / 256.0
			
			--print ("vertex buffer", vi + 15, numVertexFloats)
			
			local ii = bufferCursor * self.indicesPerChar
			for j = 1, 6 do
				indexArray [ii + j - 1] = indices [j] + bufferCursor * self.verticesPerChar
				--print (ii + j - 1, indices [j] + bufferCursor * self.verticesPerChar)
			end
			
			bufferCursor = bufferCursor + 1
			baseX = baseX + math.ceil (bakedChar.xadvance)
		end
		
		--print ("text length:", x - block.x)
	end
	
	Gl.BufferData (Gl.ARRAY_BUFFER, numVertexFloats * self.sizeOfFloat, vertexArray, Gl.STREAM_DRAW)
	Gl.BufferData (Gl.ELEMENT_ARRAY_BUFFER, numIndices * self.sizeOfInt, indexArray, Gl.STREAM_DRAW)
	
	self.numChars = numChars
end

function M:bind ()
	Gl.BindBuffer (Gl.ARRAY_BUFFER, self.bufferNames [self.VBO])
	Gl.BindBuffer (Gl.ELEMENT_ARRAY_BUFFER, self.bufferNames [self.IBO])
end

function M:vertexPointers (shader)
	Gl.VertexAttribPointer (shader.attributes.attribPos, 2, Gl.FLOAT, false, self.sizeOfFloat * self.floatsPerVertex, ffi.cast ("char *", 0))
	Gl.VertexAttribPointer (shader.attributes.attribTexCoord, 2, Gl.FLOAT, false, self.sizeOfFloat * self.floatsPerVertex, ffi.cast ("char *", 2 * self.sizeOfFloat))
end

function M:render ()
	self.numChars = self.numChars or 0
	
	Gl.DrawElements (Gl.TRIANGLES, self.numChars * self.indicesPerChar, Gl.UNSIGNED_INT, ffi.cast ("char *", 0))
end

return M