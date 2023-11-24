#include "CommonGlobals.fx"

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct VP0_In
{
	float4 tangent 	: TANGENT;
	float3 position : POSITION;
	float3 normal 	: NORMAL;
	float2 uv 		: TEXCOORD;
};
struct PP0_In
{
	float4 tangent 	: TANGENT;
	float4 position : SV_POSITION;
	float4 wPos 	: TEXCOORD0;
	float3 normal 	: NORMAL;
	float2 uv 		: TEXCOORD1;
};
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

Texture2D gAlbedoMap;
Texture2D gMetalnessMap;
Texture2D gRoughnessMap;

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
// FirstPass
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PP0_In VShaderP0(VP0_In input)
{
	PP0_In output;

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
PP0_Out PShaderP0(PP0_In input)
{
	PP0_Out output;

	output.position = input.wPos;
	output.normal = float4(input.normal, 1.0f);
    output.tangent = input.tangent;
	output.albedo = gAlbedoMap.Sample(samLinear, input.uv);
	output.metalRoughness = float2(gMetalnessMap.Sample(samLinear, input.uv).x, gRoughnessMap.Sample(samLinear, input.uv).x);

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