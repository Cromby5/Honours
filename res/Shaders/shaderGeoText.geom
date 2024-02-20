//Version number
#version 330 core

//Layout qualfier
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Passing in texture coordinates
in VS_OUT {
    vec2 texCoords;
    vec3 v_norm;
	vec4 v_pos;
} gs_in[];

// Not really sure if this is the preferred way to pass to a frag shader if needed, right now its basically the same as above, Think I might need it to texture the explode properly
// since out vec2 texCoords was needed before so im putting it here to keep things consistant and will export the normal and positons to the frag shader as well if needed for other purposes
out GS_OUT {
	vec2 texCoords;
    vec3 v_norm;
	vec4 v_pos;
} gs;


// Uniform variable
uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
//Amout of explosion
    float magnitude = 8.0;
	//Direction of explosion, going along normal
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
	//Returning position
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
//Getting the normal vector of each vertex
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   //returns the cross product between the two vectors calculated
   return normalize(cross(a, b));
}

void main()
{
//Getting normal
    vec3 normal = GetNormal();
//Setting current vertex position
    gl_Position = explode(gl_in[0].gl_Position, normal);
    gs.texCoords = gs_in[0].texCoords;
    gs.v_norm = gs_in[0].v_norm;
    gs.v_pos = gs_in[0].v_pos;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    gs.texCoords = gs_in[1].texCoords;
    gs.v_norm = gs_in[1].v_norm;
    gs.v_pos = gs_in[1].v_pos;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    gs.texCoords = gs_in[2].texCoords;
    gs.v_norm = gs_in[2].v_norm;
    gs.v_pos = gs_in[2].v_pos;
    EmitVertex();
    EndPrimitive();
}  