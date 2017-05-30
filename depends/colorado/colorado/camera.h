#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include "colorado/dllspec.h"

namespace Colorado {
class Plane;

class COLORADO_DLLSPEC Camera {
public:
	// Polar angle (up and down)
	// 0 looks down towards -Z, pi/2 looks forward to +Y, pi looks up to +Z
	float phi;
	
	// Azimuthal angle (left and right)
	// 0 looks forward, pi/2 looks right, pi looks back, pi*3/2 looks left
	float theta;
	
	glm::vec3 origin;
	
	// For orbital-style cameras
	glm::vec3 offset;
	
	// This is a ratio, not an angle. 1 will be a normal 90 degree FOV,
	// 2 will be twice as wide
	float fov;
	
	// Clipping plane distances
	float nearPlane, farPlane;
	
	Camera ();
	
	void generateFrustum (float, Plane [5]) const;
	
	glm::mat4 generateProjectionMatrix (int, int) const;
	glm::mat4 generateInvViewMatrix () const;
	glm::mat4 generateViewMatrix () const;
	glm::mat4 generateSkyViewMatrix () const;
	
	void limitPhi ();
	void limitTheta ();
	
protected:
	
};
}

#endif // CAMERA_H
