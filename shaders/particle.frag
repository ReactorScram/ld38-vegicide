uniform sampler2D color_tex;

varying lowp vec4 varyingColor;
varying mediump vec2 varyingTexCoord;

void main (void) {
	lowp vec4 color = texture2D (color_tex, varyingTexCoord);
	gl_FragColor = varyingColor * color;
}
