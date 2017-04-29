attribute vec3 attribVertexPosition;
attribute vec2 attribTexCoord;

uniform mat4 mvpMatrix;

varying mediump vec2 varyingTexCoord;

void main () {
	gl_Position = mvpMatrix * vec4 (attribVertexPosition, 1.0);
	
	varyingTexCoord = attribTexCoord;
}
