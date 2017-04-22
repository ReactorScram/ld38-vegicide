uniform sampler2D color_tex;

varying lowp vec3 varyingColor;
varying mediump vec2 varyingTexCoord;
varying lowp float varyingAoStrength;

void main (void) {
	lowp vec4 rgba = texture2D (color_tex, varyingTexCoord);
	gl_FragColor = vec4 (varyingColor * rgba.rgb, rgba.a);
}
