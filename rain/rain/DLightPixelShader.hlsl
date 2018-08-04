struct PSInput
{
	float4 Position : SV_POSITION;
};

float4 DLightPixelShader(PSInput input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}