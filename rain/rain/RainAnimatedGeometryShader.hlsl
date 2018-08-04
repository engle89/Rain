struct GSInput
{
	float3 pos              : POSITION;
	float3 seed             : SEED;
	float3 speed            : SPEED;
	float  random           : RAND;
	uint   Type             : TYPE;
};

[maxvertexcount(1)] 
void RainAnimatedGeometryShader( point GSInput input[1], inout TriangleStream< GSInput > SpriteStream)
{
	GSInput output;
	output.pos    = input[0].pos;
	output.seed   = input[0].seed;
	output.speed  = input[0].speed;
	output.random = input[0].random;
	output.Type   = input[0].Type;

	//move forward
	output.pos.xyz += input[0].speed.xyz / 144 + float3(0, -0.25, 0);

	//If the particle is outside the bounds, move it to random position near the eye
	if (output.pos.y <= -30.0)
	{
		float x = input[0].seed.x;
		float z = input[0].seed.z;
		float y = input[0].seed.y;
		output.pos = float3(x, y, z);
	}

	SpriteStream.Append(output);

	SpriteStream.RestartStrip();
}