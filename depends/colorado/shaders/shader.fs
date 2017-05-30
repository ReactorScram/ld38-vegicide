varying mediump float varyingDiffuse;
varying mediump vec2 varyingTexCoord;

uniform sampler2D texture;

void main (void) {
	mediump vec3 texColor = texture2D (texture, varyingTexCoord).rgb;
	
	gl_FragColor = vec4 (varyingDiffuse * texColor, 1.0);
}
