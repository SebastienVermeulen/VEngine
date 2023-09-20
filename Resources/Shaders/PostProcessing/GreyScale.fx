#include "../CommonGlobals.fx"

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct VP1_In
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};
struct PP1_In
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
//Textures
Texture2D gFinalRender;

//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap; // or Mirror or Clamp or Border
    AddressV = Wrap; // or Mirror or Clamp or Border
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PShader(PP1_In input) : SV_TARGET
{
    float4 color = float4(gFinalRender.Sample(samLinear, input.uv).xyz, 1.0f);
    color.xyz = (float3)((color.r + color.g + color.b) * 0.333f);
    return color;
}
										
//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VShader_Common()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PShader()));
    }
}