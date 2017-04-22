attribute mediump vec3 attribPos;
attribute lowp vec3 attribNormal;
attribute mediump vec2 attribTexCoord;

varying lowp vec3 varyingColor;
varying mediump vec2 varyingTexCoord;
varying lowp float varyingAoStrength;

uniform highp mat4 mvpMatrix;
// Object-space up
uniform lowp vec3 up;
uniform lowp vec3 diffuseColor;

void main (void) {
	gl_Position = mvpMatrix * vec4 (attribPos, 1.0);
	
	lowp float up_dot = dot (up, attribNormal);
	
	lowp float sun = clamp (up_dot, 0.0, 1.0);
	lowp float sky = mix (0.03, 1.0, up_dot * 0.5 + 0.5);
	
	varyingColor = sqrt (vec3 (sun * 0.5 + sky * 0.5) * diffuseColor * diffuseColor);
	
	varyingTexCoord = attribTexCoord * vec2 (-1.0, -1.0);
	
	varyingAoStrength = 1.0 - 0.5 * sun;
}
