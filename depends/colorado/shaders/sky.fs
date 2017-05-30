varying highp vec3 varyingRay;
varying highp vec3 cameraPosition;

uniform vec4 skyColor0;
uniform vec4 skyColor1;
uniform vec4 groundColor0;
uniform vec4 groundColor1;

uniform float scrollY;

const float groundLevel = -10.0;

void main (void) {
	// This will normally be negative
	// It makes the math a little easier
	float d = -cameraPosition.z + groundLevel;
	
	if (sign (varyingRay.z) ==  sign (d)) {
		vec2 intersection = d / varyingRay.z * varyingRay.xy + cameraPosition.xy;
		
		float fogFactor = min (length (intersection * vec2 (1.0 / 128.0, 1.0 / 256.0)), 1.0);
		// Despite the 2.0, this is actually a unit-sized check
		//float checkFactor = mod (floor (intersection.x) + floor (intersection.y), 2.0);
		
		intersection.y += scrollY;
		
		intersection *= 3.14159 * 2.0 / 10.0;
		
		float sinFactor = 0.5 * sin (intersection.x) * sin (intersection.y) + 0.5;
		
		float mixFactor = mix (sinFactor, 0.5, fogFactor);
		gl_FragColor = mix (groundColor0, groundColor1, mixFactor);
	}
	else {
		vec3 normalizedRay = normalize (varyingRay);
		float skyFactor = normalizedRay.z;
		gl_FragColor = mix (skyColor1, skyColor0, skyFactor);
	}
}
