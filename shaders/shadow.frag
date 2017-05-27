uniform sampler2D color_tex;
uniform lowp vec4 diffuseColor;

varying lowp vec4 varyingColor;
varying mediump vec2 varyingTexCoord;

void main (void) {
	lowp vec4 rgba = texture2D (color_tex, varyingTexCoord);
	gl_FragColor = vec4 (mix (vec3 (1.0), rgba.rgb * diffuseColor.rgb, rgba.a), 1.0);
}
