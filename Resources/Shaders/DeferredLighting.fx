#include "CommonGlobals.fx"
#include "CommonBRDF.fx"

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

struct LightMatrix
{
    row_major float4x4 mat;
};

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
//Lights
cbuffer LightsInfo : register(b0)
{
    int nrLights;
};
StructuredBuffer<Light> lights : register(t0);
StructuredBuffer<LightMatrix> lightsWVP : register(t1);
Texture2DArray shadowStencils : register(t2);

//Matricies
float4x4 gWorld : WORLD;
float4x4 gView : VIEW;
float4x4 gInverseView : INVERSEVIEW;
float4x4 gProjection : PROJECTION;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gInverseWorldViewProj : INVERSEWORLDVIEWPROJECTION;

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
    AddressU = Wrap; // or Mirror or Clamp or Border
    AddressV = Wrap; // or Mirror or Clamp or Border
};
SamplerState samShadow
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp; // or Mirror or Clamp or Border
    AddressV = Clamp; // or Mirror or Clamp or Border
};

DepthStencilState DepthTestDeferred
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
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

bool DepthTestShadowStencil(Light light, float4 pos)
{
    const uint shadowMapNumber = (uint)light.shadowIndex + ((uint)light.type == LIGHT_POINT ? 6 : 1);
    
    [loop]
    for (uint shadowOffset = light.shadowIndex; shadowOffset < shadowMapNumber; ++shadowOffset)
    {
        // Calculate the shadowdepth of our rendered pixel
        float4 lightSpacePos = mul(pos, lightsWVP[shadowOffset].mat);
        float3 ndc = lightSpacePos.xyz / lightSpacePos.w;
        
        // Compute texture coordinates for the current point's location on the shadow map.
        float2 shadowTexCoords = float2(0.5f, 0.5f) + ndc.xy * float2(0.5f, -0.5f);
        float pixelDepth = ndc.z;
        
        // Check if the pixel texture coordinate is in the view frustum of the 
        // light before doing any shadow work.
        if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
            (saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
            (0.0f <= pixelDepth && pixelDepth <= 1.0f))
        {
            // Sample the shadowmap
            float shadowDepth = shadowStencils.Sample(samShadow, float3(shadowTexCoords, shadowOffset)).r;
        
            const float bias = 0.001f;
            if (pixelDepth + bias < shadowDepth)
            {
                // The shadowdepth is closer than our point, so the light is blocked by other geometry
                // TO-DO: Add more settings for the shadows
                return true;
            }
        }
    }

    // Our point is not blocked by geometry
    return false;
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
    float4 pixelPos = gPositionMap.Sample(samLinear, input.uv);
    float3 normal = gNormalMap.Sample(samLinear, input.uv).rgb;
    float3 tangent = gTangentMap.Sample(samLinear, input.uv).rgb;
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
    Light light;
    float3 lightColor;
    float3 brdf;
    float cosineLaw;
    
    [unroll]
    for (uint lightIdx = 0; lightIdx < MAX_LIGHTS; ++lightIdx)
    {
        light = lights[lightIdx];
        
		//Shadow depth
        if (DepthTestShadowStencil(light, pixelPos))
        {
			//Skip light
            continue;
        }

		//Lighting calculation
        lightColor = float3(0.0f, 0.0f, 0.0f);
        brdf = float3(0.0f, 0.0f, 0.0f);
        cosineLaw = 0.0f;
        switch (light.type)
        {
        case LIGHT_DIRECTIONAL:
            lightColor = DirectionalLightBiradiance(light.color, light.intensity);
            brdf = BRDF(pixelPos.xyz, float3(gInverseView._41, gInverseView._42, gInverseView._43), normal, -light.direction.xyz, albedo.rgb, metal, rougness);
            cosineLaw = dot(normal, -light.direction.xyz);
            break;
        case LIGHT_POINT:
            float3 lightDir = normalize(light.position.xyz - pixelPos.xyz);
            lightColor = PointLightBiradiance(light.color, light.intensity, pixelPos.xyz, light.position.xyz);
            brdf = BRDF(pixelPos.xyz, float3(gInverseView._41, gInverseView._42, gInverseView._43), normal, lightDir, albedo.rgb, metal, rougness);
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

        SetVertexShader(CompileShader(vs_4_0, VShader_Common()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PShaderP1()));
    }
}
