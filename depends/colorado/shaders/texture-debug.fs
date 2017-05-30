varying highp vec2 varyingTexCoord;

void main (void) {
	highp vec3 texCoord = vec3 (varyingTexCoord.st, 1.0);
	
	gl_FragColor = vec4 (texCoord, 1.0);
}
