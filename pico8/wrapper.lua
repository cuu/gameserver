package.cpath=package.cpath..";../lua-kcp/lualib/?.so"

require 'strict'

local LEFT  = 1
local RIGHT = 2
local UP    = 3
local DOWN  = 4
local U     = 5
local I     = 6
local RETURN= 7
local ESCAPE= 8

local _player1 = {
    [1] = {[0]=LEFT,  [1]=-1},
    [2] = {[0]=RIGHT, [1] = -1},
    [3] = {[0]=UP,    [1] = -1},
    [4] = {[0]=DOWN,  [1] = -1},
    [5] = {[0]=U,     [1] = -1},
    [6] = {[0]=I,     [1] = -1},
    [7] = {[0]=RETURN,[1]=-1},
    [8] = {[0]=ESCAPE,[1]=-1},
}

local _player2 = {
    [1] = {[0]=LEFT,  [1]=-1},
    [2] = {[0]=RIGHT, [1] = -1},
    [3] = {[0]=UP,    [1] = -1},
    [4] = {[0]=DOWN,  [1] = -1},
    [5] = {[0]=U,     [1] = -1},
    [6] = {[0]=I,     [1] = -1},
    [7] = {[0]=RETURN,[1]=-1},
    [8] = {[0]=ESCAPE,[1]=-1},
}

local _player3 = {
    [1] = {[0]=LEFT,  [1]=-1},
    [2] = {[0]=RIGHT, [1] = -1},
    [3] = {[0]=UP,    [1] = -1},
    [4] = {[0]=DOWN,  [1] = -1},
    [5] = {[0]=U,     [1] = -1},
    [6] = {[0]=I,     [1] = -1},
    [7] = {[0]=RETURN,[1]=-1},
    [8] = {[0]=ESCAPE,[1]=-1},
}

local _player4 = {
    [1] = {[0]=LEFT,  [1]=-1},
    [2] = {[0]=RIGHT, [1] = -1},
    [3] = {[0]=UP,    [1] = -1},
    [4] = {[0]=DOWN,  [1] = -1},
    [5] = {[0]=U,     [1] = -1},
    [6] = {[0]=I,     [1] = -1},
    [7] = {[0]=RETURN,[1]=-1},
    [8] = {[0]=ESCAPE,[1]=-1},
}

local __keymap = {
	[0] = _player1,
  [1] = _player2,
  [2] = _player3,
  [3] = _player4
}

local coroutine_scheduler = require("coroutine_scheduler")
sched = coroutine_scheduler.Scheduler()

socket = require("socket")

function sleep(sec)
    socket.sleep(sec)
end

function time_ms() 
	return socket.gettime()*1000
end

--local api = require 'libpico8_unix_socket'
local LKcp = require "lkcp"
local LUtil = require("lutil")

local server = require("server")
local api = require("libpico8")
local lisp = require("lisp_parser")

local remote_host = "127.0.0.1"
local remote_port = 8080

local kcp1 = LKcp.lkcp_create(2, function (buf)
        udp_output1(buf, "111")
end)

kcp1:lkcp_wndsize(1024, 1024)
kcp1:lkcp_nodelay(1, 10, 2, 1)


local kcp2 = LKcp.lkcp_create(3, function (buf)
        udp_output2(buf, "112")
end)

kcp2:lkcp_wndsize(512, 512)
kcp2:lkcp_nodelay(1,10,2,1)


api.server = server 

local UDP = { data = {} ,curr_time = 0,order=0}
local udp = assert(socket.udp())

function UDP.connect()
	assert(udp:setpeername(remote_host,remote_port))
	udp:settimeout(0)
	udp:send("(ping)\n")
end

function udp_output1(buf, user)
	--print("udp_output ",#buf)
    udp:send(buf)
end

function udp_output2(buf, user)
	--print("udp_output ",#buf)
    udp:send(buf)
end

function UDP.order_send() -- must inside lua's coroutine, send with package order
  local ret,msg
  local content = ''
  
  -- print("safe_tcp_send data is " ,data ,#data)
  if #UDP.data == 0 then 
    print("UDP.send data is zero",UDP.data)
    return nil
  end

  local piece = {}
  local divid = 3
  if #UDP.data % 3 == 0 then 
    divid = 3
  end
  
  if #UDP.data % 4 == 0 then 
    divid = 4
  end  
  
  for i=1,#UDP.data,divid do 
    
    for j=1,divid do
      if UDP.data[i+j-1] ~= nil then 
        piece[j] = UDP.data[i+j-1]
      end
    end
    
    content = table.concat(piece,"|")
    package = "/pack "..tostring(UDP.order).." "..content.."\r\n"
    ret,msg = udp:send(package.."\n")
    UDP.order = UDP.order+1
  end
  
 
  UDP.data = {}
  
  return nil
end

function UDP.send() -- must inside lua's coroutine
  local ret,msg
  local content = ''
  
  -- print("safe_tcp_send data is " ,data ,#data)
  if #UDP.data == 0 then 
    print("UDP.send data is zero",UDP.data)
    return nil
  end

  local piece = {}
  local divid = 3
  if #UDP.data % 3 == 0 then 
    divid = 3
  end
  
  if #UDP.data % 4 == 0 then 
    divid = 4
  end  
  
  for i=1,#UDP.data,divid do 
    
    for j=1,divid do
      if UDP.data[i+j-1] ~= nil then 
        piece[j] = UDP.data[i+j-1]
      end
    end
    
    content = table.concat(piece,"|")
    ret,msg = udp:send(content.."\n")

  end
  
 
  UDP.data = {}
  
  return nil
end

function UDP.send_all() -- must inside lua's coroutine
  local ret,msg
  local content = ''
  
  -- print("safe_tcp_send data is " ,data ,#data)
  if #UDP.data == 0 then 
    print("UDP.send data is zero",UDP.data)
    return nil
  end
  

  content = table.concat(UDP.data,"|")
  ret,msg = udp:send(content.."\n")
  
  UDP.data = {}
  
  return nil
end


function UDP.kcp_send() 
  local ret,msg
  local content = ''
  local current = LUtil.iclock()
  -- print("safe_tcp_send data is " ,data ,#data)
  if #UDP.data == 0 then 
    print("UDP.send data is zero",UDP.data)
    return nil
  end

  kcp1:lkcp_update(current)

  local piece = {}
  local divid = 3
  if #UDP.data % 3 == 0 then 
    divid = 3
  end
  
  if #UDP.data % 4 == 0 then 
    divid = 4
  end  
  
  for i=1,#UDP.data,divid do 
    
    for j=1,divid do
      if UDP.data[i+j-1] ~= nil then 
        piece[j] = UDP.data[i+j-1]
      end
    end
    
    content = table.concat(piece,"|")
    ret,msg = kcp1:lkcp_send(content.."\n")

  end
  
  UDP.data = {}

  return nil
end


function UDP.cache(data)
  --UDP.data = UDP.data..data.."\n"
  --print(data)
  UDP.data[#UDP.data+1] = data
  
end

local TCP= {data = {},curr_time = 0}

local tcp = assert(socket.tcp())

function TCP.connect()
  local host, port = remote_host, remote_port
  tcp:connect(host, port);
  tcp:settimeout(5)
end

function TCP.send(data)
  local ret,msg
  local ret2
  -- print("safe_tcp_send data is " ,data ,#data)
  if #data == 0 then 
    print("TCP.send data is zero",data)
    return nil
  end
  
  data = data.."\n"
  
  ret,msg = tcp:send(data)
  if(ret == nil) then
	  print("exiting...",msg)
  	os.exit()
  end
  
	return nil
end


function TCP.send_all() -- must inside lua's coroutine
  local ret,msg
  local content = ''
  
  -- print("safe_tcp_send data is " ,data ,#data)
  if #TCP.data == 0 then 
    print("TCP.send_all data is zero",TCP.data)
    return nil
  end
  

  content = table.concat(TCP.data,"|")
  ret,msg = tcp:send(content.."\n")
  
  TCP.data = {}
  
  return nil
end

function TCP.cache(data)
  --print(data)
  TCP.data[#TCP.data+1] = data
  
end

log = print

local function setfenv(fn, env)
  local i = 1
  while true do
    local name = debug.getupvalue(fn, i)
    if name == "_ENV" then
      debug.upvaluejoin(fn, i, (function()
        return env
      end), 1)
      break
    elseif not name then
      break
    end

    i = i + 1
  end

  return fn
end

local function getfenv(fn)
  local i = 1
  while true do
    local name, val = debug.getupvalue(fn, i)
    if name == "_ENV" then
      return val
    elseif not name then
      break
    end
    i = i + 1
  end
end

function new_sandbox()
  return {

    clip=api.clip,
    pget=api.pget,
    pset=api.pset,

    fget=api.fget,    
    fset=api.fset,

    flip=api.flip,
    print=api.print,
    printh=log,
    cursor=api.cursor,
    color=api.color,
    cls=api.cls,
    camera=api.camera,
    circ=api.circ,
    circfill=api.circfill,
    line=api.line,
    rect=api.rect,
    rectfill=api.rectfill,
    
    pal=api.pal,
    palt=api.palt,
    spr=api.spr,
    sspr=api.sspr,
    add=api.add,
    del=api.del,
    foreach=api.foreach,
    count=api.count,
    all=api.all,
    btn=api.btn,
    btnp=api.btnp,
    sfx=api.sfx,
    music=api.music,
    
    mget=api.mget,
    mset=api.mset,
    map=api.map,

    max=api.max,
    min=api.min,
    mid=api.mid,
    flr=api.flr,
    cos=api.cos,
    sin=api.sin,
    atan2=api.atan2,
    sqrt=api.sqrt,
    abs=api.abs,
    rnd=api.rnd,
    srand=api.srand,
    sgn=api.sgn,
    band=api.band,
    bor=api.bor,
    bxor=api.bxor,
    bnot=api.bnot,
    shl=api.shl,
    shr=api.shr,
    exit=api.shutdown,
    shutdown=api.shutdown,
    sub=api.sub,
    stat=api.stat,
    time = api.time, 
		reboot = api.reboot,
		printh = api.printh,
		tostr  = api.tostr,
    mapdraw = api.map,
		run    = api.run,
		string = string,
    flip_network = api.flip_network

   }
end

function clear_keymap(keymap)
  for id=0,3 do
    for i,v in ipairs(keymap[id]) do
      keymap[id][i][1] = -1
    end
  end
end

function set_keymap(id,key,action)

	local data_array
	local pos
  local pos2
  local user_id
  
  user_id = id
  for i,v in ipairs(__keymap[user_id]) do
		if v[0] == key then
			if action == "Down" then
				__keymap[user_id][i][1] = 1
    
			elseif action == "Up" then
				__keymap[user_id][i][1] = -1
			end
      break
		end
	end 
  
  return nil

end


function GetBtnLoopUdp()
  local count = 0 
  local hrlen,hr

  while true do 
    local current = LUtil.iclock()
    kcp2:lkcp_update(current)
    while true do
      local s, status = udp:receive(1024)
      if s ~= nil then
        kcp2:lkcp_input(s)
        kcp1:lkcp_input(s)
      end
      if status == "timeout" then
        break
      end
    
      if status == "closed" then
        print("closed....")
        break
      end
    end

    while true do
      hrlen, hr = kcp2:lkcp_recv()
      if hrlen < 0 then
        break
      end
      
      local lisp_cmd = lisp.parser(hr)

      if lisp_cmd ~= nil then 
        
        if lisp_cmd.Func == "btn" then
          set_keymap(lisp_cmd.Args[1],lisp_cmd.Args[2],lisp_cmd.Args[3])
        end
        
      end
      
    end
    

    sched:suspend(udp)

  end
  
end

function draw(cart)

  local frames = 0
  local frame_time = 1/30.0
  local prev_time = 0
  local curr_time = 0
  local skip = 0  
  
  curr_time = time_ms()
  prev_time = time_ms()
  local frame_time_ms = frame_time*1000
  
  while true do
    api.pico8.frames = api.pico8.frames +1
    
    if cart._update then cart._update() end
    if cart._update60 then cart._update60() end

    if cart._draw   then cart._draw() end

      curr_time = time_ms()
      if curr_time - prev_time < frame_time_ms then
        sleep(frame_time)
       --sched:suspend(udp)
      else 
        api.flip()
        api.flip_network()
        prev_time = curr_time
      end
      
      sched:suspend(udp)
    end

end

function api.btn(i,p)
	local thing
	local ret
	
	if type(i) == 'number' then
		i = i + 1
		p = p or 0
		if __keymap[p] and __keymap[p][i] then
			ret =  __keymap[p][i][1]
			if ret >= 0 then
				return true
			else
				return false
			end
		end
	end
	
	return false

end


function api.btnp(i,p)
	local thing
	local ret

	if type(i) == 'number' then
    i= i+1
		p = p or 0
		if __keymap[p] and __keymap[p][i] then
				ret = __keymap[p][i][1]
				if ret >= 0 then
					return true
				else 
					return false
				end
		end
	end
	
	return false
	
end


function api.run()
  
end

function api.flip_network()
  
  if api.server ~= nil then
    if api.server.Network~=nil then
      api.server.Network.kcp_send()
      --api.server.Network.order_send()
    end
  end
  
end

function RunLoop(file)
  
  api.load_p8_text(file)
  
	local cart = new_sandbox()
	local ok,f,e = pcall(load,api.loaded_code)
  if not ok or f==nil then
    log('=======8<========')
    log(api.loaded_code)
    log('=======>8========')
    error('Error loading lua: '..tostring(e))
  else
		local result
		setfenv(f,cart)
    ok,result = pcall(f)
    if not ok then
      error('Error running lua: '..tostring(result))
    else
      log('lua completed')
    end

		if cart._init then cart._init() end
    
    draw(cart)
  end
end



--------------------------------------------
function main(file)

  sched:spawn(RunLoop,file)
  
  sched:spawn(GetBtnLoopUdp)
  --sched:spawn(UDP_SendLoop)

  while true do
    local worked, t = sched:select()
    if worked then
        if t and t ~= 0 then
	    print(t)
            if socket then socket.sleep(t) end
        end
    else
        print(worked, t)
        break
    end
  end


end


---------------------------------------------------
function getopt( arg, options )
  local tab = {}
  for k, v in ipairs(arg) do
    if string.sub( v, 1, 2) == "--" then
      local x = string.find( v, "=", 1, true )
      if x then tab[ string.sub( v, 3, x-1 ) ] = string.sub( v, x+1 )
      else      tab[ string.sub( v, 3 ) ] = true
      end
    elseif string.sub( v, 1, 1 ) == "-" then
      local y = 2
      local l = string.len(v)
      local jopt
      while ( y <= l ) do
        jopt = string.sub( v, y, y )
        if string.find( options, jopt, 1, true ) then
          if y < l then
            tab[ jopt ] = string.sub( v, y+1 )
            y = l
          else
            tab[ jopt ] = arg[ k + 1 ]
          end
        else
          tab[ jopt ] = true
        end
        y = y + 1
      end
    end
  end
  return tab
end


if #arg > 1 then
  local p8_file = ""
  opts = getopt( arg, "fh" )
  for k, v in pairs(opts) do
    if k == "h" then
      if #v > 1 then
        remote_host = v
      end
    end
    
    if k == "f" then 
      if #v > 2 then
        p8_file= v
      end
    end
  end

UDP.connect()
TCP.connect()

server.Network = UDP
server.NetworkTCP = TCP

  if #p8_file > 3 then
    main(p8_file)
  end

else
	log("No arguments")
	log(arg[0])
end

