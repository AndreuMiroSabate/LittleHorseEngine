Texture2D colorTex : register(t0);
SamplerState colorSampler: register(s0);

float4 main(float2 coords : TEXCOORD) : SV_TARGET
{
	//if(coords.x < 0.0 || coords.x > 1.0 || coords.y < 0.0 || coords.y > 1.0)
	//{
	//	return float4(1.0, 0.0, 1.0, 1.0); // Magenta for out-of-bounds
 //   }
	return colorTex.Sample(colorSampler, coords);
}