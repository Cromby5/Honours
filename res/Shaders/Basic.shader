#shader vertex
#version 120

attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normals;

varying vec2 texCoord0;

uniform mat4 transform;
uniform mat4 model;

void main()
{
	gl_Position = transform * vec4(position, 1.0);
	texCoord0 = texCoord;
}

#shader fragment
#version 120

varying vec2 texCoord0;

uniform sampler2D diffuse;

void main()
{
	gl_FragColor = texture2D(diffuse, texCoord0);
}
