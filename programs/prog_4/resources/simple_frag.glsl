#version 330 core 

// Simple Material Fragment Shader

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;
uniform vec3 lightColor0;

in vec3 fragNor;
in vec3 lightDir0;

// Position of the vertex in camera space
in vec3 EPos;

vec3 blinnPhong(vec3 normal, vec3 light, vec3 lightColor) {
	// Diffuse Component
	float dC = max(0.0, dot(normal, light));

	// Blinn–Phong specular
	vec3 halfway = normalize(light + normalize(-EPos));
	float ssC = pow(max(dot(normal, halfway), 0.0), MatShine);

	return lightColor * (dC*MatDif + ssC*MatSpec);
}

void main() {
	vec3 normal = normalize(fragNor);
	vec3 light0 = normalize(lightDir0);

	vec3 bpl = MatAmb + blinnPhong(normal, light0, lightColor0);
	
	color = vec4(bpl, 1.0);
}
