#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor; // huh? do I have to figure out what this is?
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 lightPos0;
uniform vec3 lightPos1;

// Keep these and set them correctly
out vec3 fragNor;
out vec3 lightDir0;
out vec3 lightDir1;
out vec3 EPos;

void main() {
	gl_Position = P * V * M * vertPos;

	// TODO: if there is no verNor, then we need to interpolate the values from 
	// all other verticies?

	fragNor = (M * vec4(vertNor, 0.0)).xyz; 
	lightDir0 = lightPos0 - (M*vertPos).xyz;
	lightDir1 = lightPos1 - (M*vertPos).xyz;
	EPos = (M*vertPos).xyz;
}
