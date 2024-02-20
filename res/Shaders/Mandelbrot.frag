#version 420 core
// Mandelbrot fragment shader
precision highp float; // All Floats will have a high level of precision

out vec4 fragColor; // Output colour of the fragment shader

// Passed in from the vertex shader
in vec2 texCoord0; 

const float MAX_ITER = 256.0; // The max amount of iterations this fractal will do before it stops, higher number = more detail but slower performance

uniform float time; // Time uniform for zoom animation, broke due to previous attempts with deltatime 

uniform float mandelTime; // Time uniform for zoom animation, not using DeltaTime

uniform sampler2D diffuse; // Texture uniform for a texture to be applied to the fractal, experimental and just for fun to see how it looks. only affects tint and the surroundings of the fractal


float mandelbrot (vec2 canvas) // Mandelbrot function, takes in a 2d vector representing the uv and returns a float
{
	vec2 c = 2 * canvas - vec2(0.5, 1); // 1st value is used to scale down the amount of coverage on the coordinates, 
									// the higher the value the smaller the overall area.
									// By subtracting a vec2 value acting as a displacment, the fractal can be moved around the plane.
	// affect c by time
	//c =  c / 0.1 * vec2(cos(time * 2), sin(time * 2)); // 
	c =  c / pow(mandelTime, 1) - vec2(0.55, 0.6); // Zooms in by time, Starts displacing by a vec2
	vec2 z = vec2(0.0); // Z starts at 0 for the first iteration of the fractal 
	for (float i = 0; i < MAX_ITER; i++)
	{
		z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c; // The mandelbrot equation. Derived from the complex number equation z = z^2 + c, where c = a + bi , our point on the canvas
		if (dot(z, z) > 2) // This bounds the values to be within the mandelbrot set, if the value is greater than x it will return as it is outside of the bounds of the set.
			return i / MAX_ITER;
	}
	return 0.0;
}

vec3 hash (float mResult) // Psuedo random hash function for colouring 3d vectors
{
	float x = fract(sin(mResult) * 67358.5453);
	float y = fract(sin(mResult + x) * 43758.5453);
	float z = fract(sin(mResult + y) * 29658.5453);
	return vec3(x, y, z);
}


void main()
{
	vec2 canvas = texCoord0.xy * 2.0 - vec2(1.0); 
	vec3 col = vec3(0.0);
	float mResult = mandelbrot(canvas);
	col += hash(mResult);
	//col = pow(col,vec3(0.45)); // Gamma correction for colour if desired, good for single colour use if hash function is not being used.
	
	fragColor = vec4(col, 1.0);
	//fragColor = vec4(col, 1.0) * texture(diffuse, texCoord0); // Texture as background / Tint
}