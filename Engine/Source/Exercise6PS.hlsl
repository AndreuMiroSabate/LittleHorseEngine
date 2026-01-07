#include "Exercise6Header.hlsli"

Texture2D diffuseTex : register(t0);
SamplerState diffuseSamp : register(s0);

float4 main(float3 worldPos : POSITION, float3 normal : NORMAL, float2 coord : TEXCOORD) : SV_TARGET
{
    float3 colorDiffuse = hasDifusseTex ? diffuseTex.Sample(diffuseSamp, coord).rgb * diffuseColour.rgb : diffuseColour.rgb;
    float3 N = normalize(normal);
    float3 V = normalize(cameraPos - worldPos);
    float3 R = reflect(lightDir, N);
    float3 satVR = saturate(dot(V, R));
    float satNL = saturate(-dot(lightDir, N));
	
    float3 colour = kDifuse * colorDiffuse * satVR * lightColor + ambientColor * colorDiffuse + kShininess * lightColor * pow(satVR, kShininess);
	
	return float4(colour, 1.0);
}