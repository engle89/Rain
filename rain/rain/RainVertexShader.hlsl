//constant buffer that can be updated every frame
cbuffer matrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 lightPosition;
};

struct VSInput
{
	float3 pos    : POSITION;
	float  random : RAND;
	float3 seed   : SEED;
	float3 speed  : SPEED;	
	uint   Type   : TYPE;
};


VSInput RainVertexShader(VSInput input)
{
	return input;
}