#include "CommonGlobals.fx"

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct VP0_Out
{
    float4 position : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
float4x4 gWorld 		: WORLD;
float4x4 gView 			: VIEW;
float4x4 gInverseView 	: INVERSEVIEW;
float4x4 gProjection 	: PROJECTION;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 

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
// TO-DO: Swap out the Vertex input with a position only model
VP0_Out VShaderP0(V_In_Shading input)
{
    VP0_Out output;	
	
	//Change the position vector to be 4 units for proper matrix calculations.
    float4 pos = float4(input.position, 1.0f);

	//Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(pos, gWorldViewProj);

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
		SetPixelShader( NULL );
	}
}
