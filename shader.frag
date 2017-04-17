uniform sampler2D color_tex;

varying lowp vec3 varyingColor;
varying mediump vec2 varyingTexCoord;
varying lowp float varyingAoStrength;

void main (void) {
	lowp float ao = mix (1.0, texture2D (color_tex, varyingTexCoord).r, varyingAoStrength);
	gl_FragColor = vec4 (varyingColor * vec3 (ao), 1.0);
}
