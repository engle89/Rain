struct VSInput
{
	float3 pos              : POSITION;   
	float  random           : RAND;
	float3 seed             : SEED;
	float3 speed            : SPEED;
	uint   Type             : TYPE;             
};

VSInput RainAnimatedVertexShader(VSInput input)
{
	//move forward
	input.pos.xyz += (input.speed.xyz / 144 + float3(0,-0.35,0));

	//If the particle is outside the bounds, move it to random position near the eye
	if (input.pos.y <= -30.0)
	{
		float x = input.seed.x;
		float z = input.seed.z;
		float y = input.seed.y;
		input.pos = float3(x, y, z);
	}

	return input;
}