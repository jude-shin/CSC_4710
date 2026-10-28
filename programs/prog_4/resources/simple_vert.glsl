#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 lightPos0;

out vec3 fragNor;
out vec3 lightDir0;
out vec3 EPos;

void main() {
	gl_Position = P * V * M * vertPos;

	fragNor = (V*M * vec4(vertNor, 0.0)).xyz; 
	EPos = (V*M*vertPos).xyz;
	lightDir0 = (V*vec4(lightPos0 - (M*vertPos).xyz, 0.0)).xyz;
}
