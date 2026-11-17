#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos0;
uniform vec3 lightPos1;

out vec2 vTexCoord;
out vec3 fragNor;
out vec3 lightDir0;
out vec3 lightDir1;
out vec3 EPos;

void main() {
	vec4 worldPos = M * vec4(vertPos, 1.0);
	vec4 viewPos = V * worldPos;

	gl_Position = P * viewPos;

	mat3 normalMatrix = transpose(inverse(mat3(V * M)));
	fragNor = normalize(normalMatrix * vertNor);

	// Both light positions must be converted into view space.
	vec3 viewLightPos0 = (V * vec4(lightPos0, 1.0)).xyz;
	vec3 viewLightPos1 = (V * vec4(lightPos1, 1.0)).xyz;

	EPos = viewPos.xyz;

	// Direction from this vertex toward each light.
	lightDir0 = viewLightPos0 - EPos;
	lightDir1 = viewLightPos1 - EPos;

	vTexCoord = vertTex;
}
