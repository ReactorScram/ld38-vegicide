attribute highp vec4 attribPos;

// Must not include projection
uniform highp mat4 inverseCameraMatrix;

// Must be height / width
uniform float aspectRatio;

varying highp vec3 varyingRay;
varying highp vec3 cameraPosition;

void main (void) {
	gl_Position = attribPos;
	
	highp vec4 viewSpaceRay = gl_Position;
	viewSpaceRay.y *= aspectRatio;
	viewSpaceRay.z = -1.0;
	viewSpaceRay.w = 0.0;
	
	gl_Position.z = 0.9999;
	
	varyingRay = (inverseCameraMatrix * viewSpaceRay).xyz;
	cameraPosition = (inverseCameraMatrix * vec4 (0.0, 0.0, 0.0, 1.0)).xyz;
	//cameraPosition *= -1.0;
}
