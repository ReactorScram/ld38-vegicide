uniform sampler2D color_tex;

varying lowp vec4 varyingColor;
varying mediump vec2 varyingTexCoord;
varying lowp float varyingAoStrength;

void main (void) {
	lowp vec4 rgba = texture2D (color_tex, varyingTexCoord);
	gl_FragColor = varyingColor * rgba;
}
