#version 330 core

out vec4 FragColor;

uniform vec4 lightColour;

void main()
{
	FragColor = lightColour;
}
