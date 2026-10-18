#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;

//position of the vertex in camera space
in vec3 EPos;

void main()
{
	//you will need to work with these for lighting
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	
	// Diffuse Component
	float dC = max(0.0, dot(normal, light));

	// Blinn–Phong specular
	vec3 halfway = normalize(light + normalize(-EPos));
	float ssC = pow(max(dot(normal, halfway), 0.0), MatShine);
	
	color = vec4(MatAmb + dC*MatDif + ssC*MatSpec, 1.0);
}
