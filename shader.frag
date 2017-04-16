varying mediump vec2 varyingTexCoord;

uniform sampler2D color_tex;

void main (void) {
	gl_FragColor = texture2D (color_tex, varyingTexCoord);
}
