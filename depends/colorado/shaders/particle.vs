// w is size
attribute vec4 attribPos;

attribute vec4 attribColor;

uniform highp mat4 mvpMatrix;

varying vec4 varyingColor;

void main (void) {
	vec4 position = vec4 (attribPos.xyz, 1.0);
	
	gl_Position = mvpMatrix * position;
	gl_PointSize = attribPos.w / gl_Position.w;
	
	varyingColor = attribColor;
}
