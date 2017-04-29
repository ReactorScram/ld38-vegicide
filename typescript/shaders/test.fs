uniform sampler2D texture;

varying mediump vec2 varyingTexCoord;

void main () {
	mediump vec4 color = texture2D (texture, varyingTexCoord);
	
	gl_FragColor = color;
}
