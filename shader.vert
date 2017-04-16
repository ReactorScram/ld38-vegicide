attribute mediump vec2 attribPos;

varying mediump vec2 varyingTexCoord;
varying mediump vec2 varyingDiagonalCoord;

void main (void) {
	gl_Position = vec4 (attribPos.x * 2.0 - 1.0, attribPos.y * 2.0 - 1.0, 1.0, 1.0);
	varyingTexCoord = vec2 (1.0) * attribPos;
	
	vec2 offset = vec2 (0.5, 0.5);
	vec2 tc = varyingTexCoord - offset;
	
	// TODO: Mat3
	varyingDiagonalCoord = vec2 (0.7 * tc.x - 0.3 * tc.y, 0.3 * tc.x + 0.7 * tc.y) + offset;
}
