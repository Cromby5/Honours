#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 normal;

uniform mat4 transform;

void main()
{
	normal = VertexNormal;
	gl_Position = transform * vec4(VertexPosition, 1.0);
}