
        local file = assert(io.open(assert(arg[1]), 'r'))

        local line, buf = 1, ''
        for s in file:lines() do
        	line = line + 1
        	if s:find'^%s*#line%s*$' then
        		buf = buf .. string.format('#line %d %q\n', line, arg[1])
        	else
        		buf = buf .. s .. '\n'
        	end
        end

        file:close()
        file = assert(io.open(arg[1], 'w'))
        file:write(buf)
        file:close()