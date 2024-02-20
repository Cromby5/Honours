#version 400

uniform vec3 lightDir;
in vec3 normal;

void main()
{
	float intensity;
	vec4 color;

	intensity = dot(lightDir, normal);

	if (intensity > 0.5)
		color = vec4(1.0,0.5,0.5,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);
	
	gl_FragColor = color;
}