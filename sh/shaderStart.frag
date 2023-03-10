#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;

in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform	float ambientStrength;;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform samplerCube skybox;
uniform int refl;

vec3 ambient;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.01f;
float shininess = 32.0f;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = ambientStrength * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = ambientStrength * specularStrength * specCoeff * lightColor;
}
float computeShadow()
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
        float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
        float currentDepth = normalizedCoords.z;
        float bias = 0.005f;
        float shadow = 
			currentDepth - bias >
			closestDepth ? 1.0f : 0.0f;
        return shadow;
}


vec3 ambientp;
float ambientStrengthp = 0.5f;
vec3 diffusePoint;
vec3 specularPoint;
float constant = 1.0f;
float linear = 0.0015f;
float quadratic = 0.0005f;
float att;

void computePointComponents()
{	float dist = length(fPosEye.xyz);
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));	
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(- vec3(fPosEye));

	att *= pow(max(dot(vec3(0.0f, 0.0f, 1.0f), lightDirN), 0.0f), 20.0f);

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	
	//compute ambient light
	ambientp = att * ambientStrengthp * lightColor;
	
	//compute diffuse light
	diffusePoint = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), 0.1f * shininess);
	specularPoint = att * specularStrength * specCoeff * lightColor;
}

float computeFog()
{
float fogDensity = 0.02f;
float fragmentDistance = length(fPosEye);
float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
return clamp(fogFactor, 0.0f, 1.0f);
}
void main() 
{
	if(refl==0){
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;
	computeLightComponents();
	
	//vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;	

	float shadow = computeShadow();
        vec3 color = min((ambient + diffuse) + specular, 1.0f);
    
	computePointComponents();
	ambientp *= texture(diffuseTexture, fTexCoords).rgb;
	diffusePoint *= texture(diffuseTexture, fTexCoords).rgb;
	specularPoint*= texture(specularTexture, fTexCoords).rgb;	
	vec3 colorp = min((ambientp + (1.0f - shadow)*diffusePoint) + (1.0f - shadow)*specularPoint, 1.0f);
	color += colorp;
	color = min(color, 1.0f);

    	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.01f, 0.01f, 0.03f, 1.0f);
	//fColor = vec4(color,colorFromTexture.a);
	fColor = mix(fogColor, vec4(color,colorFromTexture.a), fogFactor);
	}
	else{
	vec3 normalEye = normalize(fNormal);
	vec3 viewDirN = normalize(fPosEye.xyz);
	vec3 reflection = reflect(viewDirN, normalEye);

	fColor = vec4(texture(skybox, reflection).rgb, 0.5);
	}
	
}
