//constant buffer that can be updated every frame
cbuffer matrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 lightPosition;
};

struct GSInput
{
	float3 pos    : POSITION;
	float  random : RAND;
	float3 seed   : SEED;
	float3 speed  : SPEED;
	uint   Type   : TYPE;
};

struct GSOutput
{
	float4 pos           : SV_POSITION;
	float3 lightDir      : LIGHT;
	float3 pointLightDir : LIGHT2;
	float3 eyeVec        : EYE;
	float2 tex           : TEX;
	float random         : RAND;
	uint   type          : TYPE;
};

bool cullSprite(float3 position, float SpriteSize)
{
	float4 vpos = mul(float4(position,1), worldMatrix);
	vpos = mul(vpos, viewMatrix);

	if ((vpos.z < (0.1f - SpriteSize)) || (vpos.z > (100.0f + SpriteSize)))
		return true;
	else
	{
		float4 ppos = mul(vpos, projectionMatrix);
		float wext = ppos.w + SpriteSize;
		if ((ppos.x < -wext) || (ppos.x > wext) ||
			(ppos.y < -wext) || (ppos.y > wext))
			return true;
		else
			return false;
	}

	return false;
}

void GenRainSpriteVertices(float3 worldPos, float3 velVec, float3 eyePos, out float3 outPos[4])
{
	float height = 1.0 / 2.0;
	float width = height / 10.0;

	velVec = normalize(velVec);
	float3 eyeVec = eyePos - worldPos;
	float3 eyeOnVelVecPlane = eyePos - ((dot(eyeVec, velVec))*velVec);
	float3 projectedEyeVec = eyeOnVelVecPlane - worldPos;
	float3 sideVec = normalize(cross(projectedEyeVec, velVec));

	outPos[0] = worldPos  - (sideVec*0.5*width);
	outPos[1] = outPos[0] + (sideVec*width);
	outPos[2] = outPos[0] + (velVec*height);
	outPos[3] = outPos[2] + (sideVec*width);
}

float4 getProjectionPos(float4 position)
{
	position = mul(position, worldMatrix);
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);
	return position;
}

[maxvertexcount(4)]
void RainGeometryShader(point GSInput input[1], inout TriangleStream< GSOutput > SpriteStream)
{
	//float totalIntensity = g_PointLightIntensity*g_ResponsePointLight + dirLightIntensity*g_ResponseDirLight;
	float totalIntensity = 2.0 * 1.0 + 1.0 * 1.0;
	if (!cullSprite(input[0].pos, 2 * 1.0) && totalIntensity > 0)
	{
		GSOutput output;
		output.type = input[0].Type;
		output.random = input[0].random;

		float3 pos[4];
		//GenRainSpriteVertices(input[0].pos.xyz, input[0].speed.xyz/g_FrameRate + g_TotalVel, g_eyePos, pos);
		GenRainSpriteVertices(input[0].pos.xyz, input[0].speed.xyz / 144 + float3(0, -0.35, 0.0), float3(0, 0, 0), pos);

		//float3 closestPointLight = g_PointLightPos;
		float3 closestPointLight = lightPosition.xyz;
		float closestDistance = length(closestPointLight - pos[0]);

		output.pos = getProjectionPos(float4(pos[0].xyz, 1));
		output.lightDir = float3(-15, -15, -2.0) - pos[0];
		output.pointLightDir = closestPointLight - pos[0];
		output.eyeVec = float3(0, 0, 0) - pos[0];
		output.tex = float2(0, 1);
		SpriteStream.Append(output);

		output.pos = getProjectionPos(float4(pos[1].xyz,1));
		output.lightDir = float3(-15, -15, -2.0) - pos[1];
		output.pointLightDir = closestPointLight - pos[1];
		output.eyeVec = float3(0, 0, 0) - pos[1];
		output.tex = float2(1, 1);
		SpriteStream.Append(output);

		output.pos = getProjectionPos(float4(pos[2].xyz,1));
		output.lightDir = float3(-15, -15, -2.0) - pos[2];
		output.pointLightDir = closestPointLight - pos[2];
		output.eyeVec = float3(0, 0, 0) - pos[2];
		output.tex = float2(0, 0);
		SpriteStream.Append(output);

		output.pos = getProjectionPos(float4(pos[3].xyz,1));
		output.lightDir = float3(-15, -15, -2.0) - pos[3];
		output.pointLightDir = closestPointLight - pos[3];
		output.eyeVec = float3(0, 0, 0) - pos[3];
		output.tex = float2(1, 0);
		SpriteStream.Append(output);

		SpriteStream.RestartStrip();
	}
}