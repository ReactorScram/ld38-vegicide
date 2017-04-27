local M = {}

local libs = require "libcolorado"

local Colorado = libs.Colorado
local Sdl = libs.Sdl
local SdlInput = libs.SdlInput

local ffi = require "ffi"
local C = ffi.C

local coloradoDll = ffi.load "libcolorado"
local videoEncoderDll = ffi.load "libcolorado-video-encoder"

local PANDORA = libs.Colorado.COLORADO_PANDORA
local SdlFfi = ffi.load (PANDORA and "/usr/lib/libSDL-1.2.so.0" or "SDL")

local models

local graphics
local physics
local videoEncoder

local running = true
local paused = true

ffi.cdef (require "cdefs.sdl")
ffi.cdef (require "cdefs.colorado-game")
ffi.cdef (require "cdefs.colorado-video-encoder")

local checkGlError = require "Colorado.gl-error"
local lume = require "Colorado.lume"
local Input = require "input"

local GraphicsLoader = require "Colorado.graphics-loader"
local Graphics = require "graphics"
local Physics = require "physics"
local Audio = require "audio"

function M.onInit (params)
	--[[
	sh launch.sh --width 800 --height 480 -fullscreen \
	--recordAudio video/audio.bin --recordVideo video/video.mkv
	--]]
	
	local pandora = Colorado.COLORADO_PANDORA
	
	local screenOptionsP = ffi.new ("Colorado_ScreenOptions [1]")
	local screenOptions = screenOptionsP [0]
	
	if pandora then
		screenOptions.fullscreen = true
		screenOptions.width = 800
		screenOptions.height = 480
	else
		screenOptions.fullscreen = params.flags.fullscreen or false
		screenOptions.width = tonumber (params.vars.width) or 800
		screenOptions.height = tonumber (params.vars.height) or 480
	end
	
	local resourceList = (require "resource-list")(params)
	
	coloradoDll.Colorado_Lua_Game_startSdl (screenOptionsP)
	Sdl.WM_SetCaption (resourceList.windowTitle)
	
	-- Models are not owned by graphics anymore, so they can be shared with physics
	models = GraphicsLoader.loadModels (resourceList.models)
	
	checkGlError "Loaded models"
	
	do
		local shaders = GraphicsLoader.loadShaders (resourceList.shaders)
		checkGlError "Loaded shaders"
		local textures = GraphicsLoader.loadTextures (resourceList.textures)
		checkGlError "Loaded textures"
		local vbos = resourceList.loadVbos (models)
		checkGlError "Loaded VBOs"
		
		graphics = Graphics (screenOptions.width, screenOptions.height, {
			shaders = shaders,
			textures = textures,
			vbos = vbos,
		}, params) 
	end
	
	input = Input (params.flags.recordDemo, params.flags.playDemo)
	physics = Physics (models, params)
	
	local videoFn = params.recordVideo
	if videoFn and #videoFn > 0 then
		videoEncoder = videoEncoderDll.Colorado_VideoEncoder_new (videoFn, screenOptions.width, screenOptions.height)
	end
	
	Audio.init (params.vars.recordAudio)
	
	print "initialized main"
end

local function togglePause ()
	paused = not paused
	
	input:setPaused (paused)
	Audio.setPaused (paused)
end

local function processInput (inputFrame, musicFinished)
	if inputFrame.quit then
		running = false
	end
	if inputFrame.reset then
		physics = Physics (models, params)
	end
	if inputFrame.pause then
		togglePause ()
	end
	if musicFinished then
		Audio.musicFinished ()
	end
end

function M.onExec (params)
	M.onInit (params)
	
	local startTime = Sdl.getTicks ()
	local lastPhysicsTick = startTime
	
	local lastFpsTick = startTime
	local fpsFrameCount = 0
	
	local simulationTime = 0
	
	local physicsPeriod = 1000.0 / 60.0
	
	-- Ideally we'll do 1 physics iteration per frame and run at 60 FPS
	-- If we're doing 4 per frame, then the graphics are slowing us down and we're only displaying 15
	local maxPhysicsIterations = 4
	
	if params.playDemo and videoEncoder then
		maxPhysicsIterations = 1
	end
	
	togglePause ()
	
	while running do
		local currentTime = Sdl.getTicks ()
		local numPhysicsIterations = 0
		
		while lastPhysicsTick < currentTime do
			numPhysicsIterations = numPhysicsIterations + 1
			lastPhysicsTick = lastPhysicsTick + physicsPeriod
			
			if numPhysicsIterations >= maxPhysicsIterations and lastPhysicsTick < currentTime then
				-- If the game is going to drop to 15 FPS,
				-- allow the physics to slow down so we don't go into
				-- a death loop
				local simulatedTime = physicsPeriod * maxPhysicsIterations
				local unsimulatedTime = currentTime - lastPhysicsTick
				
				io.stderr:write (string.format ("Warning: Game is running at %d%% speed\n", simulatedTime / (simulatedTime + unsimulatedTime) * 100.0))
				
				lastPhysicsTick = currentTime
			end
		end
		
		if paused then
			local inputFrame, mouseRel, musicFinished = input:getInput ()
			processInput (inputFrame, musicFinished)
		else
			for i = 1, numPhysicsIterations do
				local inputFrame, mouseRel, musicFinished = input:getInput ()
				processInput (inputFrame, musicFinished)
				
				physics:step (inputFrame, mouseRel)
				local soundEvents = physics:getSoundEvents ()
				Audio.playSoundEvents (soundEvents)
				
				simulationTime = simulationTime + physicsPeriod
			end
		end
		
		if numPhysicsIterations > 0 then
			graphics:draw (physics:getFrame ())
			Colorado.swapBuffers ()
			
			if videoEncoder then
				videoEncoderDll.Colorado_VideoEncoder_accumulateFrameFromGL (videoEncoder)
				
				local time = simulationTime
				
				if not params.playDemo then
					time = currentTime - startTime
				end
				
				videoEncoderDll.Colorado_VideoEncoder_encodeAccumulatedFrame (videoEncoder, time)
			end
		else
			SdlFfi.SDL_Delay (1)
		end
		
		if params.quit then 
			running = false
		end
	end
	:: quitGame ::
	
	M.onExit ()
end

function M.onExit ()
	if videoEncoder then
		videoEncoderDll.Colorado_VideoEncoder_delete (videoEncoder)
	end
	
	Audio.exit ()
	
	coloradoDll.Colorado_Lua_Game_stopSdl ()
end

return M
