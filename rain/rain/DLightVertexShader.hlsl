cbuffer DLightBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 lightPosition;
};

struct VSInput
{
	float3 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex    : TEXCOORD;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
};

float4 getProjectionPos(float4 position)
{
	position = mul(position, worldMatrix);
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);
	return position;
}

VSOutput DLightVertexShader(VSInput input)
{
	VSOutput output;
	output.Position = (getProjectionPos(float4(input.Pos, 1))).xyww;
	return output;
}