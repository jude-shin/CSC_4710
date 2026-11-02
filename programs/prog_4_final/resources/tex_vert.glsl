#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos0;

out vec2 vTexCoord;
out vec3 fragNor;
out vec3 lightDir0;
out vec3 EPos;

void main() {
	vec4 worldPos = M * vec4(vertPos, 1.0);

	gl_Position = P * V * worldPos;

	// Keep normal and lighting data in camera/view space.
	fragNor = (V * M * vec4(vertNor, 0.0)).xyz;
	EPos = (V * worldPos).xyz;
	lightDir0 = (V * vec4(lightPos0 - worldPos.xyz, 0.0)).xyz;

	vTexCoord = vertTex;
}
