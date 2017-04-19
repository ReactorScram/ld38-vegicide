uniform sampler2D color_tex;
uniform lowp vec3 diffuseColor;

varying lowp vec3 varyingColor;
varying mediump vec2 varyingTexCoord;
varying lowp float varyingAoStrength;

void main (void) {
	gl_FragColor = vec4 (diffuseColor, 1.0);
}
