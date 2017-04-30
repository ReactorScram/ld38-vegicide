uniform sampler2D color_tex;
uniform lowp vec4 color;

varying lowp vec4 varyingColor;
varying mediump vec2 varyingTexCoord;

void main (void) {
	lowp float a = texture2D (color_tex, varyingTexCoord).a;
	gl_FragColor = vec4 (mix (vec3 (1.0), color.rgb, a), 1.0);
}
