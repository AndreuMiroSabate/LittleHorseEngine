#include "Exercise7Header.hlsli"

Texture2D diffuseTex : register(t0);
SamplerState diffuseSamp : register(s0);

float4 main(float3 worldPos : POSITION, float3 normal : NORMAL, float2 coord : TEXCOORD) : SV_TARGET
{
    float3 colorDiffuse = hasDiffuseTex ? diffuseTex.Sample(diffuseSamp, coord).rgb * diffuseColour.rgb : diffuseColour.rgb;
    float3 N = normalize(normal);
    float3 L = normalize(-lightDir);
    float3 V = normalize(cameraPos - worldPos);
    float3 R = reflect(-L, N);
    float3 satVR = saturate(dot(R, V));
    float satNL = saturate(dot(N, L));
    
    float maxRf = max(max(kSpecular.r, kSpecular.g), kSpecular.b);
    float3 fresnel = kSpecular + (1.0 - kSpecular) * pow(1.0 - satNL, 5.0);
	
    float3 colour = ((colorDiffuse*(1.0-maxRf))/PI+((kShininess + 2.0)/2*PI) * fresnel * pow(satVR, kShininess)) * lightColor * satNL;
	
    return float4(colour, 1.0);
}