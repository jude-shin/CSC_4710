#version 330 core

in vec2 texCoord;
out vec4 color;

uniform sampler2D texBuf;

void main() {
	const float stripeWidth = 15.0;
	const float horizontalShift = 9.0;

	vec2 shiftedTexCoord = texCoord;

	// Each horizontal stripe is 15 pixels tall.
	int stripeIndex = int(floor(gl_FragCoord.y / stripeWidth));

	if ((stripeIndex % 2) == 1) {
		float textureWidth = float(textureSize(texBuf, 0).x);
		shiftedTexCoord.x += horizontalShift / textureWidth;
	}

	color = texture(texBuf, shiftedTexCoord);
}
