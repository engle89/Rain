cbuffer cbPerObject : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 difColor;
	float  hasTexture;
	float  hasSpecular;
	float2 padding;
};

struct VSInput
{
	float4 pos    : POSITION;
	float2 tex    : TEXCOORD;
	float3 normal : NORMAL;
};

struct VSOutput
{
	float4 Pos      : SV_POSITION;
	float4 worldPos : POSITION;
	float2 Tex      : TEXCOORD;
	float3 Normal   : NORMAL;
};

float4 getProjectionPos(float4 position)
{
	position = mul(position, worldMatrix);
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);
	return position;
}

VSOutput ObjVertexShader(VSInput input)
{
	VSOutput output;

	output.Pos      = getProjectionPos(input.pos);
	output.worldPos = mul(input.pos, worldMatrix);
	output.Tex      = input.tex;
	output.Normal   = (mul(float4(input.normal.xyz,0), worldMatrix)).xyz;

	return output;
}