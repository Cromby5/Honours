//Version Number
#version 400

// The layout qualifers
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextCoords;
layout (location = 2) in vec3 VertexNormal;

// Uniform variable
uniform mat4 transform;
 
// Passing out the normal, TC and position data
out VS_OUT {
    vec2 texCoords;
	vec3 v_norm;
	vec4 v_pos;
} vert;

void main()
{
	// Assigning the normal, TC and position data
	vert.v_norm = VertexNormal;
	vert.texCoords = TextCoords;
	vert.v_pos = vec4(VertexPosition, 1.0);

	// Sets the position of the current vertex
	gl_Position = transform * vec4(VertexPosition, 1.0);
}

