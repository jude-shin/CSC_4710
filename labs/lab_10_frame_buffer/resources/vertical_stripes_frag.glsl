#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;

void main(){
	const float stripeWidth = 15.0;
	const float verticalShift = 9.0;

	vec2 shiftedTexCoord = texCoord;

	// Each stripe is 15 pixels wide. Shift every other stripe.
	int stripeIndex = int(floor(gl_FragCoord.x / stripeWidth));

	if ((stripeIndex % 2) == 1) {
		float textureHeight = float(textureSize(texBuf, 0).y);
		shiftedTexCoord.y += verticalShift / textureHeight;
	}

	color = texture(texBuf, shiftedTexCoord);
}
