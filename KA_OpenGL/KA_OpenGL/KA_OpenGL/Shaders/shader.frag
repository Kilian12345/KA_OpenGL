#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

// Output
out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float radius;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct OmniShadowMap
{
	samplerCube shadowMap;
	float farPlane;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightsCount;
uniform int spotLightsCount;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

// This is a 3D kernel, it is done to optimise the PCF used for OmniShadows
vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3(1,1,1),	vec3(1,-1,1),	vec3(-1,-1,1),	vec3(-1,1,1),
	vec3(1,1,-1),	vec3(1,-1,-1),	vec3(-1,-1,-1),	vec3(-1,1,-1),
	vec3(1,1,0),	vec3(1,-1,0),	vec3(-1,-1,0),	vec3(-1,1,0),
	vec3(1,0,1),	vec3(-1,0,1),	vec3(1,0,-1),	vec3(-1,0,-1),
	vec3(0,1,1),	vec3(0,-1,1),	vec3(0,-1,-1),	vec3(0,1,-1)
);

float CalcDirectionalShadowFactor(DirectionalLight light)
{
	// Perspective divide
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
	// Map between [0;1]
	projCoords = (projCoords * 0.5) + 0.5;
	
	// DirectionalLight Depth
	float current = projCoords.z;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.direction);
	
	// Bias to correct Shadow Acne
	float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.005);
			
	float shadow = 0.0;
	
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);	
	
	// 2D Shadow Kernel
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	// 3 light max (3*3)
	shadow /= 9.0;
	
	// Do not draw if the Depth is superior than 1
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	return shadow;
}

float CalcOmniShadowFactor(PointLight light, int shadowIndex)
{
	vec3 fragToLight = FragPos - light.position;
	float current = length(fragToLight);
	
	float shadow = 0.0;
	float bias = 0.05;
	float samples = 20.0;
	
	float viewDistance = length(eyePosition - FragPos);
	float diskRadius = (1.0 + (viewDistance/omniShadowMaps[shadowIndex].farPlane)) / 25.0;
	
	for(int i = 0; i < samples; i++)
	{
		float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closest *= omniShadowMaps[shadowIndex].farPlane;
		
		if(current - bias > closest)
		{
			shadow += 1.0;
		}
	}
	
	// PCF INTERPOLATION
	// PCF help us to have smoother OmniShadows
	/*for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
				closest *= omniShadowMaps[shadowIndex].farPlane;
				
				if(current - bias > closest)
				{
					shadow += 1.0;
				}
			}
		}
	}*/
	
	shadow /= float(samples);
	return shadow;

}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	vec4 ambientColor = vec4(light.color, 1.0f) * light.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColor = vec4( light.color, 1.0f) * light.diffuseIntensity * diffuseFactor; 
	
	vec4 specularColor = vec4(0,0,0,0);
	
	if(diffuseFactor> 0.0f)
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectVertex = normalize(reflect(direction, normalize(Normal)));
		
		float specularFactor = dot(fragToEye, reflectVertex);
		if(specularFactor > 0.0f && (light.ambientIntensity != 0 || light.diffuseIntensity != 0))
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor,1.0f);
		}
	}
	
	return (ambientColor + (1.0-shadowFactor) * (diffuseColor + specularColor));
};

vec4 CalcDirectionalLight()
{
	float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
};


vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
	vec3 direction = FragPos - pLight.position;
	float distance = length(direction) * (1 / pLight.radius);
	direction = normalize(direction);
	
	float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex);
	
	vec4 pointLightColor = CalcLightByDirection(pLight.base, direction, shadowFactor);
	
	float attenuation = pLight.exponent * distance * distance +
						pLight.linear * distance + 
						pLight.constant;
	
	return (pointLightColor/attenuation);

};

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)
{
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection, sLight.direction);
	
	if(slFactor > sLight.edge)
	{
		vec4 color = CalcPointLight(sLight.base, shadowIndex);
		
		return color * (1.0 - (1.0 - slFactor)*(1.0/(1.0-sLight.edge)));
	}
	else
	{
		return vec4(0,0,0,0);
	}

};

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0,0,0,0);
	
	for(int i = 0; i < pointLightsCount; i++)
	{
		totalColor += CalcPointLight(pointLights[i], i);
	}
	
	return totalColor;
};


vec4 CalcSpotLights()
{
	vec4 totalColor = vec4(0,0,0,0);
	
	for(int i = 0; i < spotLightsCount; i++)
	{
		totalColor += CalcSpotLight(spotLights[i], i + pointLightsCount);
	}
	
	return totalColor;
};

void main()
{		
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	finalColor += CalcSpotLights();
	
	color = texture(theTexture, TexCoord) * finalColor;
};	