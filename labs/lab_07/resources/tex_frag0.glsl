#version 330 core
uniform sampler2D Texture0;

uniform float MatShine;

in vec3 fragNor;
in vec3 lightDir;
in vec3 EPos;

in vec2 vTexCoord;
out vec4 Outcolor;

void main() {
	vec4 texColor0 = texture(Texture0, vTexCoord);

	// If you want to get rid of the colors
	// if (texColor0.b > texColor0.r ||
	// 		texColor0.b > texColor0.g) {
	// 	discard;
	// }

	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	float dC = max(0, dot(normal, light));

	// Outcolor = vec4(texColor0.rgb * dC, texColor0.a);
	Outcolor = texColor0;

	// To set the outcolor as the texture coordinate (for debugging)
	// Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}

