#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

// matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// pointlight
uniform vec3 pointLightPos;
uniform int pointLightOn;

float ambientPoint = 0.5f;
float specularStrengthPoint = 0.5f;
float shininessPoint = 30.0f;

float constant = 1.0f;
float linear = 0.7f;
float quadratic = 1.8f;

// spotlight
float shininess = 64.0f;

uniform vec3 spotLightPos;
uniform vec3 spotLightDirection;
uniform int spotLightOn;

float spotQuadratic = 0.02f;
float spotLinear = 0.09f;
float spotConstant = 1.0f;

uniform float spotlight1;
uniform float spotlight2;

vec3 spotLightAmbient = vec3(0.0f, 0.0f, 0.0f);
vec3 spotLightSpecular = vec3(1.0f, 1.0f, 1.0f);

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

//fog
uniform int fogOn;
uniform float fogDensity;

void computeDirLight()
{
    vec3 normalEye = normalize(fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin)
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;

}

void computePointLight(vec4 lightPosEye)
{
    vec3 cameraPosEye = vec3(0.0f);

	vec3 normalEye = normalize(fNormal);

	vec3 lightDirN = normalize(lightPosEye.xyz - fPosEye.xyz);

	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	vec3 ambientPointLight = ambientPoint * lightColor;
	vec3 diffusePointLight = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	vec3 halfVector = normalize(lightDirN + viewDirN);
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininessPoint);

	vec3 specularPointLight = specularStrengthPoint * specCoeff * lightColor;
	float distance = length(lightPosEye.xyz - fPosEye.xyz);

    vec3 intenseLightColor = vec3(1.0, 1.0, 0.5);

    ambientPointLight = intenseLightColor * ambientPointLight * 2.0;

    diffusePointLight = intenseLightColor * diffusePointLight * 2.0;

    specularPointLight = intenseLightColor * specularPointLight *2.0;

	float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);

    ambient+= ambientPointLight * att;
    diffuse+= diffusePointLight* att;
    specular+= specularPointLight * att;
    
}

void computeLightSpotComponents() {
	vec3 cameraPosEye = vec3(0.0f);
	vec3 lightDir = normalize(spotLightPos - fPosition);
	vec3 normalEye = normalize(fNormal);
    
	vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	float diff = max(dot(fNormal, lightDir), 0.0f);
	float spec = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	float distance = length(spotLightPos - fPosition);
	float attenuation = 1.0f / (spotConstant + spotLinear * distance + spotQuadratic * distance * distance);

	float theta = dot(lightDir, normalize(-spotLightDirection));
	float epsilon = spotlight1 - spotlight2;
	float intensity = clamp((theta - spotlight2)/epsilon, 0.0, 1.0);

    vec3 intenseLightColor = vec3(0.0, 1.0, 0.0);

	vec3 ambientSpotLight = intenseLightColor * spotLightAmbient;
	vec3 diffuseSpotLight = intenseLightColor * spotLightSpecular * diff;
	vec3 specularSpotLight = intenseLightColor * spotLightSpecular * spec;
	ambientSpotLight *= attenuation * intensity;
	diffuseSpotLight *= attenuation * intensity;
	specularSpotLight *= attenuation * intensity;
	
	ambient+= ambientSpotLight;
    diffuse+= diffuseSpotLight;
    specular+= specularSpotLight;
}

float computeShadow() {
    vec3 normalizedCoords = fragPosLightSpace.xyz/fragPosLightSpace.w; //[-1,1]
	normalizedCoords = normalizedCoords * 0.5f + 0.5f;

	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;


	float bias = 0.005f;

	float shadow =  currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	if(normalizedCoords.z>1.0f) return 0.0f;

	return shadow;
}

float computeFog()
{

 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    computeDirLight();

    if(pointLightOn==1)
    {
       vec4 lightPosEye = view * vec4(pointLightPos, 1.0f);
       computePointLight(lightPosEye);
    }

    if(spotLightOn==1)
    {
       computeLightSpotComponents();
    }

    ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);

    fColor = vec4(color, 1.0f);

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	if (fogOn == 1)
	{
		fColor = fogColor * (1 - fogFactor) + vec4(color, 1.0f) * fogFactor;
	}

}
