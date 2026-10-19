#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

// uniform float ambIntensity;
// uniform float difIntensity;
// uniform float specIntensity;

// Interpolated normal and light vector in camera space
in vec3 fragNor; // Will always have something (vertex shader passes normal to the fragment (this shader))
in vec3 lightDir0;
in vec3 lightDir1;

// Position of the vertex in camera space
in vec3 EPos;

vec3 blinnPhong(vec3 normal, vec3 light) {
	// Diffuse Component
	float dC = max(0.0, dot(normal, light));

	// Blinn–Phong specular
	vec3 halfway = normalize(light + normalize(-EPos));
	float ssC = pow(max(dot(normal, halfway), 0.0), MatShine);

	// TODO: have a vec3 input that represents all three of the intensities?
	// OR just pass in one scalar for every one of the intensities 
	// (ambient, diffuse, specular)
	return dC*MatDif + ssC*MatSpec;
}

void main() {
	vec3 normal = normalize(fragNor);
	vec3 light0 = normalize(lightDir0);
	vec3 light1 = normalize(lightDir1);

	vec3 bpl = MatAmb + blinnPhong(normal, light0) + blinnPhong(normal, light1);
	
	color = vec4(bpl, 1.0);
}
