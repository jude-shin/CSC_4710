#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 lightPos;

// Keep these and set them correctly
out vec3 fragNor;
out vec3 lightDir;
out vec3 EPos;

void main() {
	gl_Position = P * V * M * vertPos;

	// Update these as needed

	// TODO: if there is no verNor, then we need to interpolate the values from 
	// all other verticies?

	fragNor = (M * vec4(vertNor, 0.0)).xyz; 
	lightDir = lightPos - (M*vertPos).xyz;
	EPos = (M*vertPos).xyz;

}
