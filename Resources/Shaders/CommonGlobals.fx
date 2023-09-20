//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define MAX_LIGHTS 10
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
float gPI = 3.14159265359f;

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct V_In_Common
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};
struct P_In_Common
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
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
