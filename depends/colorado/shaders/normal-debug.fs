varying highp vec3 varyingNormal;

void main (void) {
	gl_FragColor = vec4 (varyingNormal.xyz, 1.0);
}
