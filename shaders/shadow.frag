uniform sampler2D color_tex;
uniform lowp vec3 diffuseColor;

varying lowp vec3 varyingColor;
varying mediump vec2 varyingTexCoord;
varying lowp float varyingAoStrength;

void main (void) {
	vec4 rgba = texture2D (color_tex, varyingTexCoord);
	gl_FragColor = vec4 (mix (vec3 (1.0), rgba.rgb * diffuseColor, rgba.a), 1.0);
}
