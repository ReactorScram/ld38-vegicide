local M = {}

function M.makeTableAndWriter ()
	local t = {}
	local i = 1
	
	local function write (value)
		t [i] = value
		i = i + 1
	end
	
	return t, write
end

return M