local M = {}

function M.parse (args)
	local result = {
		-- Anything that doesn't fit into a flag or var
		-- Sequence
		pos = {},
		-- Anything preceded by one dash
		-- Set
		flags = {},
		-- Anything preceded by two dashes
		-- Map
		vars = {},
	}
	
	local nextVar = nil
	
	for _, arg in ipairs (args) do
		local flagMatch = arg:match ("^%-(.*)")
		local varMatch = arg:match ("^%-%-(.*)")
		
		if nextVar then
			result.vars [nextVar] = arg
			nextVar = nil
		elseif varMatch then
			nextVar = varMatch
		elseif flagMatch then
			result.flags [flagMatch] = true
		else
			table.insert (result.pos, arg)
		end
	end
	
	return result
end

function M.dump (parsedArgs)
	print "Flags"
	for i, _ in pairs (parsedArgs.flags) do
		print (i)
	end
	
	print "Vars"
	for i, v in pairs (parsedArgs.vars) do
		print (i, "=", v)
	end
	
	print "Positional"
	for i, v in ipairs (parsedArgs.pos) do
		print (i, v)
	end
end

return M