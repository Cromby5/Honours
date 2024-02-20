#version 420 core

// The code for this shader may be fine, 
// Its appearence may seem weird due to using skybox code that differs from what was done in class 
// As technically this shader contains most of what was done for reflection extention last trimester
// You can see that it is mixing the colour as intended by the lab instructions to prevent 100% reflection, 
// that is seen in the previous reflectshader from gp2.

// For This reason the original reflect shader is included for a side by side comparison.


// Create a vec4 and send it out as our final colour
out vec4 fragColour;
// Read in the interface block from the fragment shader
// Vertex_DATA / vs_out
in Vertex_DATA
{
	vec2 tC;
    vec3 Normal;
    vec3 Position;
} vert_In;

// Now create 3 uniforms, one for our camera position, one for our skybox, and one for a 2D texture. Note if you remove the explicit binding 
// (layout (binding = 1)) the console will output an error even although the shader will work. 
// It is good practice to fix warnings and errors as they can potentially lead to issues as applications develop.
uniform vec3 cameraPos;
uniform samplerCube skybox;
layout (binding = 1) uniform sampler2D diffuse;

void main()
{

	// Calculate the vector from the camera to the fragment.
	vec3 I = normalize(vert_In.Position - cameraPos); //vector to camera

	// Calculate the reflection vector using the vertex normal as the angle of incident.
	vec3 R = reflect(I, normalize(vert_In.Normal)); //calculate the reflection direction for the incident vector

	// Use this vector to read the rgb value from the samplerCube (our skybox)
	vec4 Colour = vec4(texture(skybox, R).rgb, 1.0);
	// Now read the rgb values from our texture2D
	vec4 texColour = vec4(texture2D(diffuse, vert_In.tC).rgb, 1.0);
	// and mix the result with “Colour”. You will need to use “glGetUniformLocation” application side, which we covered in a previous lab.
	
	fragColour = mix(Colour, texColour, 0.5);
}