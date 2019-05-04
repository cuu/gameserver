local lisp = {}

local function starts_with(str, start)
   return str:sub(1, #start) == start
end

local function ends_with(str, ending)
   return ending == "" or str:sub(-#ending) == ending
end

function lisp.parser(lisp_str)
	
	local segs = {}
	local depth = 0
	local instring = 0
	local lastpos = 0
	local segs_number=0
	local LispCmd = {Func="",Args={}}

	for i = 1, #lisp_str do
    local c = lisp_str:sub(i,i)
		if c ~= ' ' and c ~= '(' then
			if depth ==0 then
				print("syntax error ",i,lisp_str)
				return
			end
		end

		if c == '(' then
			depth = depth +1
			lastpos = i
		end
		
		if c == ')' then
			if lastpos < i then	
				local tmp = lisp_str:sub(lastpos+1,i-1)
				segs[segs_number] = tmp
				segs_number = segs_number+1
			end

			depth = depth -1
		end
		
		if depth > 0 then
			if c == ' ' then
				if instring == 0 then
					local tmp = lisp_str:sub(lastpos+1,i-1)
					
					segs[segs_number] = tmp
					segs_number = segs_number + 1
					
					lastpos = i
				end
			end
		end
		
		if c == '"' then
			if instring == 0 then
				instring = instring +1
			elseif instring > 0 then
				instring = instring -1
			end
		end	
	end
	
	if depth > 0 then
		print("syntax error ,unexcepted closure")
		return nil
	end

	if instring > 0 then
		print("syntax error , string quato errors")
		return nil
	end

	if segs_number < 1 then
		print("unknown error")
		return nil
	end
	
	LispCmd.Func = segs[0]

	for i,v in ipairs(segs) do
		if starts_with(v,'"') and ends_with(v,'"') then
			segs[i] = segs[i]:sub(2,#segs[i]-1)
		end
	end
	LispCmd.Args = segs
	
	return LispCmd
end

return lisp

--[[

function foo(a,b)
	print("i am foo", a,  b)
end

funcs = {}
funcs["foo"] = foo

l = lisp_parser("(foo 1 \"hah\")")

funcs[l.Func](l.Args[1],l.Args[2])

]]

