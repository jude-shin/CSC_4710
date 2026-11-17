// skybox_frag.glsl
#version 330 core

uniform sampler2D Texture0;

in vec2 vTexCoord;
out vec4 Outcolor;

void main() {
	Outcolor = texture(Texture0, vTexCoord);
}
