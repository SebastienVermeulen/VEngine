#include "CommonGlobals.fx"

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct VP1_In
{
	float3 position : POSITION;
	float2 uv 		: TEXCOORD;
};
struct PP1_In
{
	float4 position : SV_POSITION;
	float2 uv 		: TEXCOORD;
};
struct Light
{
	float4 position;
	float4 direction;
	float3 color;
	float intensity;
	int type;
	bool enabled;
	float2 padding;
};

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
//Lights
cbuffer Lights : register(b0)
{
	Light lights[MAX_LIGHTS];
};

//Matricies
float4x4 gWorld : WORLD;
float4x4 gView : VIEW;
float4x4 gInverseView : INVERSEVIEW;
float4x4 gProjection : PROJECTION;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

//Textures
Texture2D gPositionMap;
Texture2D gNormalMap;
Texture2D gTangentMap;
Texture2D gBinormalMap;

Texture2D gAlbedoMap;
Texture2D gMetalRoughnessMap;

//Global math vars
float3 gBackGroundColor = float3(0.0f, 0.2f, 0.4f);
float gPhongExponent = 60.0f;
float gAlbedoMultiply = 1.0f;
float gMetalnessMultiply = 1.0f;
float gRoughnessMultiply = 1.0f;

//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Culling
{
	CullMode = BACK;
};
DepthStencilState DepthTest
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};
DepthStencilState DepthTestDeferred
{
	DepthEnable = FALSE;
	DepthWriteMask = ALL;
};

//--------------------------------------------------------------------------------------
// Helper Func
//--------------------------------------------------------------------------------------
float4 Phong(float3 normal, float3 camDir, float3 lightDir)
{
	float lambert = pow(dot((-lightDir + 2.0f * (dot(normal, lightDir)) * normal), camDir), gPhongExponent);
	return (float4)lambert;
}
float3 Lambert(float3 diffReflect, float3 diffColor)
{
	return (diffColor * diffReflect) / (float)gPI;
}
float TrowbridgeReitz(float3 halfVector, float3 normal, float rougness) //Normal distribution
{
	float a = rougness * rougness;
	float dotNH = dot(normal, halfVector);
	float div = (dotNH * dotNH) * (a - 1.0f) + 1.0f;
	return a / (gPI * div * div); 
}
float3 Fresnel(float3 halfVector, float3 camDir, float3 albedo) //Fresnel Schlick
{
	float oneMinusDotHV = 1.0f - dot(halfVector, camDir);
	return albedo + (1.0f - albedo) * (oneMinusDotHV * oneMinusDotHV * oneMinusDotHV * oneMinusDotHV * oneMinusDotHV);
}
float Schlick(float3 dir, float3 normal, float rougness) //Geometry Schlick-GGX
{
	float dotNV = max(dot(normal, dir), 0.0f);
	float kDirect = (rougness + 1.0f) * (rougness + 1.0f) / 8.0f;
	//float kIndirect = rougness * rougness / 2.0f;
	return dotNV / (dotNV * (1.0f - kDirect) + kDirect);
}
float3 BRDF(float3 pixelPos, float3 normal, float3 lightDirection, float3 albedo, float metal, float rougness)
{
	//Metalness influence
	float3 metalnessColor = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metal);

	//Helping variables
	float3 camDir = normalize(float3(gInverseView._41, gInverseView._42, gInverseView._43) - pixelPos);
	float3 halfVector = (camDir + lightDirection) / length(camDir + lightDirection);

	//Specular calculations
	float D = TrowbridgeReitz(halfVector, normal, rougness);
	float3 F = Fresnel(halfVector, camDir, metalnessColor);
	float G = Schlick(lightDirection, normal, rougness) * Schlick(camDir, normal, rougness); //Smith's method
	float3 cookTorrance = (D * F * G) / (4.0f * dot(lightDirection, normal) * dot(camDir, normal));

	//Factors
	float3 kd = (float3)1.0f - F;

	//Diffuse
	float3 lambert = Lambert(kd, albedo);

	//Final
	return lambert * kd + cookTorrance;
}

float3 PointLightBiradiance(float3 lightColor, float lightIntensity, float3 pos, float3 lightPos)
{
	float3 posToLight = lightPos - pos;
	return lightColor * (lightIntensity / dot(posToLight, posToLight));
}
float3 DirectionalLightBiradiance(float3 dirLightColor, float dirLightIntensity)
{
	return dirLightColor * dirLightIntensity;
}

//--------------------------------------------------------------------------------------
// FirstPass
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PP1_In VShaderP1(VP1_In input)
{
	PP1_In output;

	// Change the position vector to be 4 units for proper matrix calculations.
    output.position = float4(input.position, 1.0f);
	output.uv = input.uv;

	return output;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PShaderP1(PP1_In input) : SV_TARGET
{
	//Buffer variables
	float3 pixelPos = gPositionMap.Sample(samLinear, input.uv).xyz;
	float3 normal = gNormalMap.Sample(samLinear, input.uv);
	float3 tangent = gTangentMap.Sample(samLinear, input.uv);
    float3 binormal = gBinormalMap.Sample(samLinear, input.uv);
	float4 albedo = gAlbedoMap.Sample(samLinear, input.uv);
	albedo = float4(min(1.0f, gAlbedoMultiply * albedo.rgb), albedo.a);
    float2 metalRoughness = gMetalRoughnessMap.Sample(samLinear, input.uv).rg;
    float metal = min(1.0f, gMetalnessMultiply * metalRoughness.x);
    float rougness = min(1.0f, gRoughnessMultiply * metalRoughness.y);

	//Clearbuffer .a is set to 0, in the first pass pixelshader it is set to 1.0f
    if (!all(albedo.a))
	{
		return float4(gBackGroundColor, 1.0f);
	}
	float3 color = float3(0.0f, 0.0f, 0.0f);

	//For each light do the lighting calculation
	for(int lightIdx = 0; lightIdx < MAX_LIGHTS; ++lightIdx)
	{
		Light light = lights[lightIdx];
		if(!light.enabled)
		{
			//Skip light
			continue;
		}

		//Lighting calculation
		float3 lightColor = float3(0.0f, 0.0f, 0.0f);
		float3 brdf = float3(0.0f, 0.0f, 0.0f);
		float cosineLaw = 0.0f;
		switch(light.type)
		{
		case LIGHT_DIRECTIONAL:
			lightColor = DirectionalLightBiradiance(light.color, light.intensity);
			brdf = BRDF(pixelPos, normal, light.direction.xyz, albedo.rgb, metal, rougness);
			cosineLaw = dot(normal, light.direction.xyz);
			break;
		case LIGHT_POINT:
			float3 lightDir = normalize(light.position.xyz - pixelPos);
			lightColor = PointLightBiradiance(light.color, light.intensity, pixelPos, light.position.xyz);
            brdf = BRDF(pixelPos, normal, lightDir, albedo.rgb, metal, rougness);
			cosineLaw = dot(normal, lightDir);
			break;
		}
        color += max(lightColor * brdf * cosineLaw, 0.0f);
    }
	
    return float4(saturate(color), 1.0f);
}
										
//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VShaderP1() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PShaderP1() ) );
	}
}