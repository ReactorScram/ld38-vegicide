#version 120

uniform sampler2D texture;

varying vec4 varyingColor;

void main (void) {
	gl_FragColor = varyingColor * texture2D (texture, gl_PointCoord.st);
}
