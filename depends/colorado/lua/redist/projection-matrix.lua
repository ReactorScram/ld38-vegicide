local Matrix = require "glua.matrix"

local libs = libs or require "libcolorado"

local Colorado = libs.Colorado

local M = {}

M.idealAspect = 480.0 / 800.0

function M.makeProjectionMatrix (width, height, near, far, fovDegrees)
	-- This aspect ratio was handed down by the OpenPandora team
	local idealAspect = M.idealAspect
	
	local aspect = height / width
	local near = near or 0.5
	local far = far or 150.0
	local fov
	local fovDegrees = fovDegrees or 90
	local baseFov = math.tan (fovDegrees * math.pi / 180 / 2)
	
	if idealAspect < aspect then
		-- Screen is too tall, add extra pixels on top and bottom
		fov = baseFov * near
	else
		-- Screen is too wide, add extra pixels on left and right
		fov = baseFov * idealAspect / aspect * near
	end
	
	local projMat = Matrix.frustum (-fov, fov, -aspect * fov, aspect * fov, near, far)
	
	return projMat
end

function M.makeProjectionMatrixColorado (width, height, near, far, 
fovDegrees, idealAspect)
	-- This aspect ratio was handed down by the OpenPandora team
	local idealAspect = idealAspect or M.idealAspect
	
	local aspect = height / width
	local near = near or 2.0
	local far = far or 5000.0
	local fov
	local fovDegrees = fovDegrees or 75.0
	local baseFov = math.tan (fovDegrees * math.pi / 180 / 2)
	
	if idealAspect < aspect then
		-- Screen is too tall, add extra pixels on top and bottom
		fov = baseFov * near
	else
		-- Screen is too wide, add extra pixels on left and right
		fov = baseFov * idealAspect / aspect * near
	end
	
	local projMat = Colorado.newMatrix ()
	projMat:frustum (-fov, fov, -aspect * fov, aspect * fov, near, far)
	
	return projMat
end

return M
