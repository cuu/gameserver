local audio = {}
local al = require('moonal')
local bit = require('bit')
local osc = {}

local __buffer_count = 8
local __buffer_size = 1024
local __sample_rate = 22050
local channels = 1
local bits = 16

local flr = math.floor
local unpack = table.unpack
local abs = math.abs

local function oldosc(osc)
    local x = 0
    return function(freq)
        x = x + freq / __sample_rate
        return osc(x)
    end
end

local function lerp(a, b, t)
    return (b - a) * t + a
end

local function note_to_hz(note)
    return 440 * 2 ^ ((note - 33) / 12)
end

function audio.load(pico8)
    local device = al.open_device()
    local context = al.create_context(device)
    pico8.audio_source = al.create_source(context)
    --pico8.audio_source:play()
    pico8.audio_buffers = {}
    pico8.audio_source:set('velocity', {1, 1, 10})

    pico8.audio_buffer = {}
    pico8.audio_context = context
		for i=1,8 do 
			local buf = al.create_buffer(pico8.audio_context)
			print(buf)
			pico8.audio_buffers[i] = buf
		end

    -- tri
    osc[0] = function(x)
        local t = x % 1
        return (abs(t * 2 - 1) * 2 - 1) * 0.5
    end
    -- uneven tri
    osc[1] = function(x)
        local t = x % 1
        return (((t < 0.875) and (t * 16 / 7) or ((1 - t) * 16)) - 1) * 0.5
    end
    -- saw
    osc[2] = function(x)
        return (x % 1 - 0.5) * 2 / 3
    end
    -- sqr
    osc[3] = function(x)
        return (x % 1 < 0.5 and 1 or -1) * 0.25
    end
    -- pulse
    osc[4] = function(x)
        return (x % 1 < 0.3125 and 1 or -1) * 0.25
    end
    -- organ
    osc[5] = function(x)
        x = x * 4
        return (abs((x % 2) - 1) - 0.5 + (abs(((x * 0.5) % 2) - 1) - 0.5) / 2 - 0.1) * 0.5
    end
    osc[6] = function()
        local lastx = 0
        local sample = 0
        local update = false
        local hz48 = note_to_hz(48)
        return function(x)
            local hz = ((x - lastx) % 1) * __sample_rate
            lastx = x
            local scale = hz * (131072 / 343042875) + (16 / 889)

            update = not update
            if update then
                sample = sample + scale * (math.random() * 2 - 1)
            end
            local output = sample * (45 / 32)
            if hz > hz48 then
                output =
                    output *
                    (1.1659377442658412e+000 - 2.3350687035974510e-004 * hz + 8.3385655344351036e-008 * hz ^ 2 -
                        1.1509506025078735e-011 * hz ^ 3) -- approximate
            end
            sample = math.max(math.min(sample, (6143 / 31115)), -(6143 / 31115))
            return output
        end
    end
    -- detuned tri
    osc[7] = function(x)
        x = x * 2
        return (abs(((x * 127 / 128) % 2) - 1) / 2 + abs((x % 2) - 1) - 1) * 2 / 3
    end
    -- saw from 0 to 1, used for arppregiator
    osc['saw_lfo'] = function(x)
        return x % 1
    end

    for i = 0, 3 do
        pico8.audio_channels[i] = {
            oscpos = 0,
            sample = 0,
            noise = osc[6]()
        }
    end
end

function audio.update(api, pico8)
    -- check what sfx should be playing

    for bufferpos = 0, __buffer_size - 1 do
        if pico8.current_music then
            pico8.current_music.offset =
								pico8.current_music.offset + 7350 / (61 * pico8.current_music.speed * __sample_rate)
						
						--print(pico8.current_music.music, " ",pico8.current_music.offset)
            if pico8.current_music.offset >= 32 then
                local next_track = pico8.current_music.music
                if pico8.music[next_track].loop == 2 then
                    -- go back until we find the loop start
                    while true do
                        if pico8.music[next_track].loop == 1 or next_track == 0 then
                            break
                        end
                        next_track = next_track - 1
                    end
                elseif pico8.music[pico8.current_music.music].loop == 4 then
                    next_track = nil
                elseif pico8.music[pico8.current_music.music].loop <= 1 then
                    next_track = next_track + 1
                end
                if next_track then
                    api.music(next_track)
                end
            end
        end
        local music = pico8.current_music and pico8.music[pico8.current_music.music] or nil

        local sample = 0
        for channel = 0, 3 do
            local ch = pico8.audio_channels[channel]
            local note, instr, vol, fx
            local freq

            if ch.sfx and pico8.sfx[ch.sfx] then
                local sfx = pico8.sfx[ch.sfx]
                ch.offset = ch.offset + 7350 / (61 * sfx.speed * __sample_rate)
                if sfx.loop_end ~= 0 and ch.offset >= sfx.loop_end then
                    if ch.loop then
                        ch.last_step = -1
                        ch.offset = sfx.loop_start
                    else
                        pico8.audio_channels[channel].sfx = nil
                    end
                elseif ch.offset >= 32 then
                    pico8.audio_channels[channel].sfx = nil
                end
            end
            if ch.sfx and pico8.sfx[ch.sfx] then
                local sfx = pico8.sfx[ch.sfx]
                -- when we pass a new step
                if flr(ch.offset) > ch.last_step then
                    ch.lastnote = ch.note
                    ch.note, ch.instr, ch.vol, ch.fx = unpack(sfx[flr(ch.offset)])
                    if ch.instr ~= 6 then
                        ch.osc = osc[ch.instr]
                    else
                        ch.osc = ch.noise
                    end
                    if ch.fx == 2 then
                        ch.lfo = oldosc(osc[0])
                    elseif ch.fx >= 6 then
                        ch.lfo = oldosc(osc['saw_lfo'])
                    end
                    if ch.vol > 0 then
                        ch.freq = note_to_hz(ch.note)
                    end
                    ch.last_step = flr(ch.offset)
                end
                if ch.vol and ch.vol > 0 then
                    local vol = ch.vol
                    if ch.fx == 1 then
                        -- slide from previous note over the length of a step
                        ch.freq = lerp(note_to_hz(ch.lastnote or 0), note_to_hz(ch.note), ch.offset % 1)
                    elseif ch.fx == 2 then
                        -- vibrato one semitone?
                        ch.freq = lerp(note_to_hz(ch.note), note_to_hz(ch.note + 0.5), ch.lfo(8))
                    elseif ch.fx == 3 then
                        -- drop/bomb slide from note to c-0
                        local off = ch.offset % 1
                        -- local freq=lerp(note_to_hz(ch.note), note_to_hz(0), off)
                        local freq = lerp(note_to_hz(ch.note), 0, off)
                        ch.freq = freq
                    elseif ch.fx == 4 then
                        -- fade in
                        vol = lerp(0, ch.vol, ch.offset % 1)
                    elseif ch.fx == 5 then
                        -- fade out
                        vol = lerp(ch.vol, 0, ch.offset % 1)
                    elseif ch.fx == 6 then
                        -- fast appreggio over 4 steps
                        local off = bit.band(flr(ch.offset), 0xfc)
                        local lfo = flr(ch.lfo(sfx.speed <= 8 and 16 or 8) * 4)
                        off = off + lfo
                        local note = sfx[flr(off)][1]
                        ch.freq = note_to_hz(note)
                    elseif ch.fx == 7 then
                        -- slow appreggio over 4 steps
                        local off = bit.band(flr(ch.offset), 0xfc)
                        local lfo = flr(ch.lfo(sfx.speed <= 8 and 8 or 4) * 4)
                        off = off + lfo
                        local note = sfx[flr(off)][1]
                        ch.freq = note_to_hz(note)
                    end
                    ch.sample = ch.osc(ch.oscpos) * vol / 7
                    ch.oscpos = ch.oscpos + ch.freq / __sample_rate
                else
                    ch.sample = 0
                end
            else
                ch.sample = 0
            end
            sample = sample + ch.sample
        end
        -- PICO-8 limits max volume to 80%, but since picolove is quieter anyway we opt for increasing the volume
        --buffer:setSample(bufferpos, math.min(math.max(sample * 1.25, -1), 1))
        pico8.audio_buffer[bufferpos] = math.min(math.max(sample * 1.25, -1), 1)
        --print(pico8.audio_buffer[bufferpos]   )
    end
end

function audio.run(api, pico8)
    audio.update(api, api.pico8)
    --print("audio update")

end

return audio
