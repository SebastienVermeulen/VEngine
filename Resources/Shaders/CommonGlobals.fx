
#ifndef COMMON_GLOBALS
#define COMMON_GLOBALS 1

//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define MAX_LIGHTS 10
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

#define PI 3.14159265359f

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct Light
{
    float4 position;
    float4 direction;
    float3 color;
    float intensity;
    int type;
    int shadowIndex; // We need an index in case not all lights shadowcast and we are offset
    float2 padding;
};

struct V_In_Common
{
    float3 position : POSITION;
    float2 uv       : TEXCOORD;
};
struct P_In_Common
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};
struct V_In_Shading
{
    float4 tangent  : TANGENT;
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};
struct P_In_Shading
{
    float4 tangent  : TANGENT;
    float4 position : SV_POSITION;
    float4 wPos     : TEXCOORD0;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------
RasterizerState Culling_Common
{
	CullMode = BACK;
};
DepthStencilState DepthTestNone_Common
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

BlendState NoBlending
{
    BlendEnable[0] = FALSE;
    RenderTargetWriteMask[0] = 0x0F; // Write to all RGBA channels (D3D11_COLOR_WRITE_ENABLE_ALL)
};

//----------------------------------------
// Vertex Shaders
//----------------------------------------
P_In_Common VShader_Common(V_In_Common input)
{
    P_In_Common output;

	// Change the position vector to be 4 units for proper matrix calculations.
    output.position = float4(input.position, 1.0f);
    output.uv = input.uv;

    return output;
}
#endif // COMMON_GLOBALS
