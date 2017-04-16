uniform sampler2D color_tex;

varying lowp vec3 varyingColor;

void main (void) {
	gl_FragColor = vec4 (varyingColor, 1.0);
}
