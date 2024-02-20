#version 330 core

out vec4 FragColour;

in vec2 texCoord0;

in vec3 Normal;
in vec3 currentPos;

uniform sampler2D diffuse;
uniform sampler2D normalMap;
uniform sampler2D spec;

uniform vec4 lightColour;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
	float ambient = 0.20f; // Adding an ambient light value
	// Diffuse Lighting
	vec3 normal = normalize(Normal);
	//NORMAL MAP APPLICATION test
	//vec3 normal = normalize(texture(normalMap, texCoord0).xyz *2.0f -1.0f);
	vec3 lightDirection = normalize(lightPos - currentPos);
	
	float diffuseLight = max(dot(normal,lightDirection), 0.0f);
	
	// Specular Lighting 
	float specularLight = 0.50f; 
	vec3 viewDir = normalize(cameraPos - currentPos);
	vec3 reflectDir = reflect(-lightDirection,normal);
	float specularAmount = pow(max(dot(viewDir,reflectDir),0.0f),8);
	float specular = specularAmount * specularLight;
	
	//FragColour = texture(diffuse, texCoord0) * lightColour * (diffuseLight + ambient + specular);
	FragColour = texture(diffuse, texCoord0) * lightColour * (diffuseLight + ambient) +  texture(spec, texCoord0).r * specular;
}
