static const float PI = 3.14159265;

cbuffer PerFrame : register(b1)
{
    float3 lightDir;
    float3 lightColor;
    float3 ambientColor;
    float3 cameraPos;
}

cbuffer PerInstance : register(b2)
{
    float4x4 modelMat;
    float4x4 normalMat;
    
    float3 diffuseColour;
    bool hasDiffuseTex;
    float3 kSpecular;
    float kShininess;
    
}