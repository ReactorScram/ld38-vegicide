varying mediump vec2 varyingTexCoord;
varying mediump vec2 varyingDiagonalCoord;

uniform sampler2D color_tex;

void main (void) {
	float red = texture2D (color_tex, varyingTexCoord).r;
	float green = texture2D (color_tex, varyingDiagonalCoord).g;
	
	float yellow = 1.0 - red * green;
	
	//yellow = pow (yellow, 0.5);
	
	float debug_mix = 0.0;
	
	gl_FragColor = vec4 (yellow + red * debug_mix, yellow + green * debug_mix, yellow, 1.0);
}
