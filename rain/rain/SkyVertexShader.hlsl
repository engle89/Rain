cbuffer skyBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix WVP;
};

struct VSInput
{
	float3 Pos      : POSITION;
};

struct VSOutput
{
	float4 HPos     : SV_POSITION;
	float3 Tex      : TEXCOORD;
};

float4 getProjectionPos(float4 position)
{
	position = mul(position, worldMatrix);
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);
	return position;
}

VSOutput SkyVertexShader(VSInput input)
{
	VSOutput output;
	//output.HPos = mul(float4(input.Pos.xyz, 1.0f), WVP).xyww;
	output.HPos = (getProjectionPos(float4(input.Pos.xyz, 1.0f))).xyww;
	output.Tex = input.Pos;

	return output;
}
