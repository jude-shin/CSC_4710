#version 330 core

uniform sampler2D Texture0;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;
uniform vec3 lightColor0;

in vec2 vTexCoord;
in vec3 fragNor;
in vec3 lightDir0;
in vec3 EPos;

out vec4 Outcolor;

vec3 blinnPhong(vec3 normal, vec3 light, vec3 diffuseColor) {
	float dC = max(0.0, dot(normal, light));

	vec3 halfway = normalize(light + normalize(-EPos));
	float ssC = pow(max(dot(normal, halfway), 0.0), MatShine);

	return lightColor0 * (dC * diffuseColor + ssC * MatSpec);
}

void main() {
	vec4 texColor0 = texture(Texture0, vTexCoord);

	vec3 normal = normalize(fragNor);
	vec3 light0 = normalize(lightDir0);

	// Texture replaces the original diffuse material color.
	vec3 texturedDiffuse = texColor0.rgb * MatDif;

	vec3 shadedColor = texColor0.rgb	* MatAmb + blinnPhong(normal, light0, texturedDiffuse);

	Outcolor = vec4(shadedColor, texColor0.a);
}
