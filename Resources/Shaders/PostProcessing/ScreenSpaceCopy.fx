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

struct TargetRect
{
    // Ranged 0 - 1
    float x, y;
    float height, width;
};

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
//Textures
Texture2D gInputTexture;
TargetRect gInputTargetRect;
TargetRect gOutputTargetRect;

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
float4 FullCopyPShader(PP1_In input) : SV_TARGET
{
    return float4(gInputTexture.Sample(samLinear, input.uv).xyz, 1.0f);
}

float4 TargetedCopyPShader(PP1_In input) : SV_TARGET
{
    float2 currentUV = input.uv;

    if (currentUV.x < gOutputTargetRect.x ||
        currentUV.y < gOutputTargetRect.y ||
        currentUV.x > gOutputTargetRect.x + gOutputTargetRect.width ||
        currentUV.y < gOutputTargetRect.y + gOutputTargetRect.height)
    {
        discard;
    }
    
    float2 outputLocalUV = float2((currentUV.x - gOutputTargetRect.x) / gOutputTargetRect.width, (currentUV.y - gOutputTargetRect.y) / gOutputTargetRect.height);
    float2 inputLocalUV = float2((outputLocalUV.x - gInputTargetRect.x) / gInputTargetRect.width, (outputLocalUV.y - gInputTargetRect.y) / gInputTargetRect.height);
    
    return float4(gInputTexture.Sample(samLinear, inputLocalUV).xyz, 1.0f);
}
										
//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 FullCopy
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VShader_Common()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, FullCopyPShader()));
    }
}
technique11 TargetedCopy
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VShader_Common()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TargetedCopyPShader()));
    }
}