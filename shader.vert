attribute mediump vec2 attribPos;

varying mediump vec2 varyingTexCoord;

void main (void) {
	gl_Position = vec4 (attribPos.x * 2.0 - 1.0, attribPos.y * 2.0 - 1.0, 1.0, 1.0);
	varyingTexCoord = attribPos;
}
