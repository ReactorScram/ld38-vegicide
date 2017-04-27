attribute highp vec4 attribPos;
attribute highp vec3 attribNormal;
attribute highp float attribMatrixIndex;
attribute highp vec2 attribTexCoord;

uniform highp mat4 mvpMatrix [4];
uniform highp mat3 normalMatrix [4];
varying highp vec3 varyingNormal;

void main (void) {
	highp int matrixIndex = int (attribMatrixIndex);
	
	gl_Position = mvpMatrix [matrixIndex] * attribPos;
	
	varyingNormal = normalMatrix [matrixIndex] * attribNormal;
}
