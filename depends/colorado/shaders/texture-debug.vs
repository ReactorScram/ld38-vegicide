attribute highp vec4 attribPos;
attribute highp vec3 attribNormal;
attribute highp float attribMatrixIndex;
attribute highp vec2 attribTexCoord;

// TODO: Why are these 29 and not 32?
uniform highp mat4 mvpMatrix [29];
uniform highp mat3 normalMatrix [29];

varying highp vec2 varyingTexCoord;

void main (void) {
	highp int matrixIndex = int (attribMatrixIndex);

	gl_Position = mvpMatrix [matrixIndex] * attribPos;
	
	varyingTexCoord = attribTexCoord;
}
