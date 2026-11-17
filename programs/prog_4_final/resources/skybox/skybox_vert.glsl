// skybox_vert.glsl
#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec3 vertPos;
in vec2 vertTex;

out vec2 vTexCoord;

void main() {
	vTexCoord = vertTex;
	vec4 position = P * V * M * vec4(vertPos, 1.0);
	gl_Position = position.xyww;
}
