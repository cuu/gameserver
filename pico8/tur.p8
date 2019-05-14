pico-8 cartridge // http://www.pico-8.com
version 16
__lua__
-- two turtles

x1 = 15
y1 = 104
speed = 2

x2 = 104
y2 = 104

function refresh()
	
	cls()
	
	map()
		
	spr(1,x1,y1)
	spr(5,x2,y2)
	
end

function buttons1()
	if btn(0,0) then
		x1 = x1-speed
	end
	
	if btn(1,0) then
		x1 = x1+speed
	end
	
	if btn(2,0) then
		y1 = y1-speed
	end
	
	if btn(3,0) then
		y1 = y1+speed
	end
	
	if x1 > 128 then
		x1 = x1 - 128
	end
	if x1 < 0 then
		x1 = x1+128
	end
	if y1 > 128 then
		y1 = y1 - 128
	end
	if y1 < 0 then
		y1 = y1+128
	end
	
end

function buttons2()
	if btn(0,1) then
		x2 = x2-speed
	end
	
	if btn(1,1) then
		x2 = x2 + speed
	end
	if btn(2,1) then
		y2 = y2 - speed
	end
	
	if btn(3,1) then
		y2 = y2 + speed
	end

	if x2 > 128 then
		x2 = x2 - 128
	end
	if x2 < 0 then
		x2 = x2 + 128
	end
	
	if y2 > 128 then
		y2 = y2 - 128
	end
	if y2 < 0 then
		y2 = y2 + 128
	end
	
end

function _update()
	refresh()
	buttons1()
	buttons2()
end



__gfx__
0000000000088000000bb0000000000000bbb0000000800000000000000000000000000000000000000000000000000000000000000000000000000000000000
000000000888888000bbbb000e00000e0bbbbbbb0088880000000000000000000000000000000000000000000000000000000000000000000000000000000000
00700700000880000bb00bb00ee000e0bb0b00bb0899998000000000000000000000000000000000000000000000000000000000000000000000000000000000
00077000088888800b0000b0000e0ee0bbb000b00809098000000000000000000000000000000000000000000000000000000000000000000000000000000000
00077000088888800b0000b00000e000bbb000bb0899998000000000000000000000000000000000000000000000000000000000000000000000000000000000
00700700008888000bbbbbb00000ee00b0bbbbbb0889098000000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000088888800bb00bb000ee0ee0b00000b00090090000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000000008000080bb0000bb0ee0000e0bbbbb000900009000000000000000000000000000000000000000000000000000000000000000000000000000000000
__map__
0400000000030003030303030303000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0303030303030303030303030303000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0003030303030003030303030303030000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0004000303030303030303030303030000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0000030303030303030303030403000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0303030303030303000303030403000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0303030303030303030303030303030000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0403030303030303030303030303040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0403030303030000000003030303040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0400000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0400000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0400000000000000000000000000040400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0400000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0404040404040404040404040404040400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0303000303030303030303030303030300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
0303030303030303030303030303030300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
