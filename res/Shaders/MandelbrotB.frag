#version 420 core
// Mandelbrot fragment
precision highp float; 
out vec4 fragColor;

// CHANGE to the array thing, vs_out
in vec2 texCoord0;
in vec3 Normal;
in vec3 currentPos;

uniform float time;
uniform sampler2D diffuse;

#define MAX_ITERATIONS 256 // Const value for max iterations

int get_iterations()
{
	// Can be combined into a single vec2 for xy
	float realNum = ((texCoord0.x - 0.6)) * 4.0; 
	float imagineNum = ((texCoord0.y - 0.7)) * 4.0;
	// move by time
	int i = 0;
	float x = realNum;
	float y = imagineNum;

	while (i < MAX_ITERATIONS)
	{
		float xtemp = realNum;
		realNum = (realNum * realNum - imagineNum * imagineNum) + realNum; // (x * x - y * y) + x
		imagineNum = (4.0 * xtemp * imagineNum) + y; // (4 * x * y) + y

		if (realNum * realNum + imagineNum * imagineNum > 2.0)
			break;
			
		i++;
	}
	return i;
}

vec4 get_color()
{
    int iter = get_iterations();
    if (iter == MAX_ITERATIONS)
    {
        gl_FragDepth = 0.0f;
        return vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
 
    float iterations = float(iter) / MAX_ITERATIONS;    
    return vec4(0.0f, iterations, 0.0f, 1.0f);
}

void main()
{
	fragColor = get_color();
}


