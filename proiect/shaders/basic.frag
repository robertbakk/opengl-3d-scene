#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 lightDir2;
uniform vec3 lightColor2;
uniform float fogDensity;


// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
vec3 ambient2;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
vec3 diffuse2;
vec3 specular2;
vec4 fPosEye;
float specularStrength = 0.5f;

float computeFog()
{
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}


void computeDirLight()
{
    //compute eye space coordinates
    fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));
	vec3 lightDirN2 = vec3(normalize(view * vec4(lightDir2, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;
	ambient2 = ambientStrength * lightColor2;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	diffuse2 = max(dot(normalEye, lightDirN2), 0.0f) * lightColor2;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 reflectDir2 = reflect(-lightDirN2, normalEye);
	float specCoeff2 = pow(max(dot(viewDir, reflectDir2), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
	specular2 = specularStrength * specCoeff2 * lightColor2;
}

void main() 
{
    computeDirLight();
	float fogFactor = computeFog();
	vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);

    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
	vec3 color2 = min((ambient2 + diffuse2) * texture(diffuseTexture, fTexCoords).rgb + specular2 * texture(specularTexture, fTexCoords).rgb, 1.0f);

    fColor = vec4(mix(fogColor, color+color2, fogFactor), 1.0f);

}
