#include "CommonGlobals.fx"
#include "CommonBRDF.fx"

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct PP0_Out
{
	float4 position 		: SV_Target0;
	float4 normal 			: SV_Target1;
	float4 tangent 			: SV_Target2;
	float4 albedo 			: SV_Target3;
	float2 metalRoughness	: SV_Target4;
};

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
float4x4 gWorld 		: WORLD;
float4x4 gView 			: VIEW;
float4x4 gInverseView 	: INVERSEVIEW;
float4x4 gProjection 	: PROJECTION;
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
BlendState AlphaBlendingOn
{
    BlendEnable[0] = TRUE;
    DestBlend = INV_SRC_ALPHA;
    SrcBlend = SRC_ALPHA;
};

//--------------------------------------------------------------------------------------
// Helper Func
//--------------------------------------------------------------------------------------
void GatherParams(in float2 uv, out float4 albedo, out float metal, out float roughness)
{
#if defined(ALBEDO_SCALAR_PARAMETER)
	albedo = gObjectColor;
#else
    albedo = gAlbedoMap.Sample(samLinear, uv);
#endif //defined(ALBEDO_SCALAR_PARAMETER)
	
#if defined(ALBEDO_SCALAR_PARAMETER)
    metal = gObjectMetal;
#else
    metal = gMetalnessMap.Sample(samLinear, uv);
#endif //defined(ALBEDO_SCALAR_PARAMETER)
	
#if defined(ROUGHNESS_SCALAR_PARAMETER)
    roughness = gObjectRoughness;
#else
    roughness = gRoughnessMap.Sample(samLinear, uv);
#endif //defined(ROUGHNESS_SCALAR_PARAMETER)

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
    output.normal = normalize(mul(input.normal, (float3x3) gWorld));
    output.tangent = input.tangent;
	
	output.uv = input.uv;

	return output;
}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PP0_Out PShaderP0(P_In_Shading input)
{
	PP0_Out output;

	output.position = input.wPos;
	output.normal = float4(input.normal, 1.0f);
    output.tangent = input.tangent;
    GatherParams(input.uv, output.albedo, output.metalRoughness.x, output.metalRoughness.y);

	return output;
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
