#version 400

uniform mat4 u_pm;
uniform mat4 u_vm;
uniform vec3 lightDir;

in vec3 normal;
 
layout( location = 0 ) out vec4 fragcolor;
 
in vec3 v_norm;
in vec4 v_pos; 
 
void main() {
 
  vec3 n = normalize(mat3(u_vm) * v_norm);      // convert normal to view space
  vec3 p = vec3((u_pm) * v_pos);                // position in clip space
  vec3 v = normalize(p);                        // normalised eye vector
  float vdn = max(dot(v, n), 0.0);        // the rim contribution

	float intensity;
	vec4 color; //toon

	intensity = dot(lightDir, normal);

	if (intensity > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);

  fragcolor.a = 0.6;
  fragcolor.rgb = vec3(smoothstep(0.8, 0.9, vdn)) + vec3(color.x,color.y,color.z);

}