#include "CommonGlobals.fx"

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
static const float gPhongExponent = 60.0f;

//--------------------------------------------------------------------------------------
// Phong
//--------------------------------------------------------------------------------------
float4 Phong(float3 normal, float3 camDir, float3 lightDir)
{
    float distribution = pow(dot((-lightDir + 2.0f * dot(normal, lightDir) * normal), camDir), gPhongExponent);
    return (float4) distribution;
}

//--------------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------------
float TrowbridgeReitz(float3 halfVector, float3 normal, float rougness) //Normal distribution
{
    float a = rougness * rougness;
    float dotNH = saturate(dot(normal, halfVector));
    float distribution = (dotNH * dotNH) * (a - 1.0f) + 1.0f;
    return a / max(gPI * distribution * distribution, 0.000001f);
}
float3 Fresnel(float3 halfVector, float3 camDir, float3 f0) //Fresnel Schlick
{
    float oneMinusDotHV = 1.0f - saturate(dot(halfVector, camDir));
    return f0 + (1.0f - f0) * pow(oneMinusDotHV, 5);
}
float Schlick(float3 dir, float3 normal, float rougness) //Geometry Schlick-GGX
{
    float dotNV = max(dot(normal, dir), 0.000001f);
    float k = rougness / 2.0f;
    return dotNV / (dotNV * (1.0f - k) + k);
}

//--------------------------------------------------------------------------------------
// CookTorrance BRDF
//--------------------------------------------------------------------------------------
float3 BRDF(float3 pixelPos, float3 viewPos, float3 normal, float3 lightDirection, float3 albedo, float metal, float rougness)
{
    //Diffuse calculations
    float3 diffuse = albedo / gPI;
    
    //Helping variables
    float3 invViewDir = normalize(viewPos - pixelPos);
    float3 halfVector = normalize(invViewDir + lightDirection);
    float3 F0 = lerp((float3) 0.04f, albedo, metal);
    
    //Specular calculations
    float D = TrowbridgeReitz(halfVector, normal, rougness);
    float3 F = Fresnel(halfVector, invViewDir, F0);
    float G = Schlick(lightDirection, normal, rougness) * Schlick(invViewDir, normal, rougness); //Smith's method
    float3 cookTorrance = (D * F * G) / (4.0f * max(dot(lightDirection, normal), 0.000001f) * max(dot(invViewDir, normal), 0.000001f));

    //final
    float kd = ((1.0f - F) * (1.0f - metal)); // F = ks
    return kd * diffuse + cookTorrance;
}
