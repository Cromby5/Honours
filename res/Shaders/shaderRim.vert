#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec3 VertexNormal;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;

out vec3 v_norm;
out vec4 v_pos; 
out vec3 normal;

void main()
{
	normal = VertexNormal; //static

	v_norm = mat3(transpose(inverse(view*model))) * VertexNormal;
	v_pos = vec4(VertexPosition, 1.0);
	gl_Position = transform * vec4(VertexPosition, 1.0);
}