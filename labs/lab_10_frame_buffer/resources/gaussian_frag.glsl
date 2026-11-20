#version 330 core

in vec2 texCoord;
out vec4 color;

uniform sampler2D texBuf;

void main() {
	const float kernel[25] = float[](
			1.0,  4.0,  6.0,  4.0, 1.0,
			4.0, 16.0, 24.0, 16.0, 4.0,
			6.0, 24.0, 36.0, 24.0, 6.0,
			4.0, 16.0, 24.0, 16.0, 4.0,
			1.0,  4.0,  6.0,  4.0, 1.0
			);

	vec2 texelSize = 1.0 / vec2(textureSize(texBuf, 0));
	vec3 blurredColor = vec3(0.0);

	for (int y = -2; y <= 2; y++) {
		for (int x = -2; x <= 2; x++) {
			int kernelIndex = (y + 2) * 5 + (x + 2);
			float weight = kernel[kernelIndex];

			vec2 sampleCoord =
				texCoord + vec2(float(x), float(y)) * texelSize;

			blurredColor += texture(texBuf, sampleCoord).rgb * weight;
		}
	}

	color = vec4(blurredColor / 256.0, 1.0);
}
