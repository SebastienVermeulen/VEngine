#include "CommonGlobals.fx"
#include "CommonBRDF.fx"

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

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
//Lights
cbuffer Lights : register(b0)
{
    Light lights[gMaxLights];
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

Texture2D gAlbedoMap;
Texture2D gMetalRoughnessMap;

//Global math vars
float3 gBackGroundColor = float3(0.0f, 0.2f, 0.4f);

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

DepthStencilState DepthTestDeferred
{
	DepthEnable = FALSE;
	DepthWriteMask = ALL;
};

//--------------------------------------------------------------------------------------
// Helper Func
//--------------------------------------------------------------------------------------
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
// Common alternative
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PShaderP1(PP1_In input) : SV_TARGET
{
	//Buffer variables
	float3 pixelPos = gPositionMap.Sample(samLinear, input.uv).xyz;
	float3 normal = gNormalMap.Sample(samLinear, input.uv);
	float3 tangent = gTangentMap.Sample(samLinear, input.uv);
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
	for(int lightIdx = 0; lightIdx < gMaxLights; ++lightIdx)
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
		case gLightDirectional:
			lightColor = DirectionalLightBiradiance(light.color, light.intensity);
            brdf = BRDF(pixelPos, float3(gInverseView._41, gInverseView._42, gInverseView._43), normal, light.direction.xyz, albedo.rgb, metal, rougness);
			cosineLaw = dot(normal, light.direction.xyz);
			break;
		case gLightPoint:
			float3 lightDir = normalize(light.position.xyz - pixelPos);
			lightColor = PointLightBiradiance(light.color, light.intensity, pixelPos, light.position.xyz);
            brdf = BRDF(pixelPos, float3(gInverseView._41, gInverseView._42, gInverseView._43), normal, lightDir, albedo.rgb, metal, rougness);
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
		SetRasterizerState(Culling_Common);
		SetDepthStencilState(DepthTestDeferred, 0);

		SetVertexShader( CompileShader( vs_4_0, VShader_Common() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PShaderP1() ) );
	}
}
