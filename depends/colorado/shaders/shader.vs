attribute mediump vec4 attribPos;
attribute mediump vec3 attribNormal;
attribute mediump float attribMatrixIndex;
attribute mediump vec2 attribTexCoord;

const mediump int NUM_SUBNODES = 12;

uniform mediump mat4 mvpMatrix [NUM_SUBNODES];
uniform mediump mat3 normalMatrix [NUM_SUBNODES];

varying mediump float varyingDiffuse;
varying mediump vec2 varyingTexCoord;

const mediump vec3 lightVec = vec3 (-0.43301, 0.25, 0.86602);
const mediump float AMBIENT_MIN = 0.125;

void main (void) {	
	mediump int matrixIndex = int (attribMatrixIndex);
	
	gl_Position = mvpMatrix [matrixIndex] * attribPos;
	
	mediump vec3 normal = normalMatrix [matrixIndex] * attribNormal;
	
	varyingDiffuse = max (AMBIENT_MIN, dot (normal, lightVec));
	
	varyingTexCoord = attribTexCoord;
}
