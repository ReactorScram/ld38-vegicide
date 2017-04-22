attribute mediump vec3 attribPos;
attribute lowp vec3 attribNormal;
attribute mediump vec2 attribTexCoord;

varying lowp vec4 varyingColor;
varying mediump vec2 varyingTexCoord;

uniform highp mat4 mvpMatrix;
uniform lowp vec4 diffuseColor;

void main (void) {
	gl_Position = mvpMatrix * vec4 (attribPos, 1.0);
	
	varyingColor = diffuseColor;
	
	varyingTexCoord = attribTexCoord * vec2 (1.0, -1.0);
}