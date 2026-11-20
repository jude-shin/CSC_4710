#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;

void main(){

	float squareSize = 10.0;
	float period = 20.0;

	vec2 cellPos = mod(gl_FragCoord.xy, period);

	vec3 sceneColor = texture(texBuf, texCoord).rgb;

	if (cellPos.x < squareSize && cellPos.y < squareSize) {
		vec3 blueColor = vec3(0.0, 0.0, 0.75);
		float opacity = 0.5;

		vec3 blendedColor = mix(sceneColor, blueColor, opacity);
		color = vec4(blendedColor, 1.0);
	} else {
		color = vec4(sceneColor, 1.0);
	}
}
