//sampler
SamplerState SkySamplerState : register(s0);

//skymap
TextureCube SkyMap : register(t0);

struct PSInput
{
	float4 HPos     : SV_POSITION;
	float3 Tex      : TEXCOORD;
};

float4 SkyPixelShader(PSInput input) : SV_TARGET
{
	float4 color = SkyMap.Sample(SkySamplerState, input.Tex);
	if (color.x > 0.50 && color.y > 0.50)
		return float4(1, 1, 1, 1) * 20;

	return color;
}
