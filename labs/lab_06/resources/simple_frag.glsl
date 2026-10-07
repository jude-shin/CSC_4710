#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;

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

	float dC = max(dot(normal, light), 0.0);
	vec3 finalColor = MatAmb + dC * MatDif;
	
	color = vec4(finalColor, 1.0);
}
