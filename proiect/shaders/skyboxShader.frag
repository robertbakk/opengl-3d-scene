#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;
uniform float fogDensity;

void main()
{
	vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
	float density = fogDensity;
	if (density > 0.005)
		density = 0.005;
    color = vec4(200*density*fogColor + (1-200*density)*texture(skybox, textureCoordinates).xyz, 1.0f);
}
