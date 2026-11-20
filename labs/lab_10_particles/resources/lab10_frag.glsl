#version 330 core

uniform sampler2D alphaTexture;

in vec4 partCol; // Changed from vec3 to vec4

out vec4 outColor;

void main() {
	float alpha = texture(alphaTexture, gl_PointCoord).r;

	// Combine the sprite's texture alpha with the particle's individual alpha
	outColor = vec4(partCol.rgb, alpha * partCol.a);
}
