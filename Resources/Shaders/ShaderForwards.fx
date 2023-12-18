#include "CommonGlobals.fx"
#include "CommonBRDF.fx"

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
#if defined(ALBEDO_SCALAR_PARAMETER)
	float4 gObjectColor;
#else
	Texture2D gAlbedoMap;
#endif //defined(ALBEDO_SCALAR_PARAMETER)
#if defined(ALBEDO_SCALAR_PARAMETER)
	float gObjectMetal;
#else
	Texture2D gMetalnessMap;
#endif //defined(ALBEDO_SCALAR_PARAMETER)
#if defined(ROUGHNESS_SCALAR_PARAMETER)
	float gObjectRoughness;
#else
	Texture2D gRoughnessMap;
#endif //defined(ROUGHNESS_SCALAR_PARAMETER)

//Global math vars
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

DepthStencilState DepthTest
{
	DepthEnable = TRUE;
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

void GatherParams(in float2 uv, out float4 albedo, out float metal, out float roughness)
{
#if defined(ALBEDO_SCALAR_PARAMETER)
	albedo = gObjectColor;
#else
    albedo = gAlbedoMap.Sample(samLinear, uv);
#endif //defined(ALBEDO_SCALAR_PARAMETER)
    albedo = float4(min(1.0f, gAlbedoMultiply * albedo.rgb), albedo.a);
	
#if defined(ALBEDO_SCALAR_PARAMETER)
    metal = gObjectMetal;
#else
    metal = gMetalnessMap.Sample(samLinear, uv).r;
#endif //defined(ALBEDO_SCALAR_PARAMETER)
    metal = min(1.0f, gMetalnessMultiply * metal);
	
#if defined(ROUGHNESS_SCALAR_PARAMETER)
    roughness = gObjectRoughness;
#else
    roughness = gRoughnessMap.Sample(samLinear, uv).r;
#endif //defined(ROUGHNESS_SCALAR_PARAMETER)
    roughness = min(1.0f, gRoughnessMultiply * roughness);
}

//--------------------------------------------------------------------------------------
// FirstPass
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
P_In_Shading VShaderP0(V_In_Shading input)
{
    P_In_Shading output;

	//Change the position vector to be 4 units for proper matrix calculations.
    output.wPos = float4(input.position, 1.0f);

	//Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(output.wPos, gWorldViewProj);
    output.wPos = mul(output.wPos, gWorld);
	
	//Calculate the normal and tangent of the vertex against the world rotation and scale.
    output.normal = mul(input.normal, (float3x3)gWorld);
    output.tangent = input.tangent;
	
	output.uv = input.uv;

	return output;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PShaderP0(P_In_Shading input) : SV_TARGET
{
	//Buffer variables
    float3 pixelPos = input.wPos.xyz;
	float3 normal = input.normal;
	float3 tangent = input.tangent;
	
    float4 albedo = (float4)0.0f;
    float metal = 0.0f;
    float rougness = 0.0f;
    GatherParams(input.uv, albedo, metal, rougness);

	float3 color = float3(0.0f, 0.0f, 0.0f);

	//For each light do the lighting calculation
    for (int lightIdx = 0; lightIdx < gMaxLights; ++lightIdx)
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
            brdf = BRDF(pixelPos, float3(gInverseView._41, gInverseView._42, gInverseView._43), normal, light.direction.xyz, albedo.xyz, metal, rougness);
			cosineLaw = dot(normal, light.direction.xyz);
			break;
        case gLightPoint:
			float3 lightDir = normalize(light.position.xyz - pixelPos);
			lightColor = PointLightBiradiance(light.color, light.intensity, pixelPos, light.position.xyz);
            brdf = BRDF(pixelPos, float3(gInverseView._41, gInverseView._42, gInverseView._43), normal, lightDir, albedo.xyz, metal, rougness);
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
		SetDepthStencilState(DepthTest, 0);

		SetVertexShader( CompileShader( vs_4_0, VShaderP0() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PShaderP0() ) );
	}
}
