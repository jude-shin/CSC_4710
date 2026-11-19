#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;

void main(){

	vec3 texColor = texture( texBuf, texCoord ).rgb;
	//TODO modify to show this is a 2D image
	color = vec4(texColor, 1.0);

}
