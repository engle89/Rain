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

struct Light
{
	float3 pos;
	float  range;
	float3 dir;
	float  cone;
	float3 att;
	float padding;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame : register(b1)
{
	Light light;
}

struct PSInput
{
	float4 Pos      : SV_POSITION;
	float4 worldPos : POSITION;
	float2 Tex      : TEXCOORD;
	float3 Normal   : NORMAL;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 SLightPixelShader(PSInput input) : SV_TARGET
{
	input.Normal = normalize(input.Normal);

   //Set diffuse color of material
   float4 diffuse = difColor;

   //If material has a diffuse texture map, set it now
   if (hasTexture == true)
       diffuse = ObjTexture.Sample(ObjSamplerState, input.Tex);

   float3 finalColor = float3(0.0f, 0.0f, 0.0f);

   float3 lightToPixelVec = light.pos - input.worldPos;
   float d = length(lightToPixelVec);
   float3 finalAmbient = diffuse * light.ambient;

   if (d > light.range)
       return float4(finalAmbient, diffuse.a);

   lightToPixelVec /= d;
   float howMuchLight = dot(lightToPixelVec, input.Normal);
   if (howMuchLight > 0.0f)
   {
	   finalColor += diffuse * light.diffuse;
	   finalColor /= (light.att[0] + (light.att[1] * d) + (light.att[2] * (d*d)));
	   finalColor *= pow((max(dot(-lightToPixelVec, light.dir), 0.0f)), light.cone);
	   finalColor *= ((light.cone + 8) / 8);
   }

   finalColor = saturate(finalColor + finalAmbient);

   //If this is a lamp, make it emmissive
   if (diffuse.x > 0.95 && diffuse.y > 0.95 && diffuse.z > 0.95)
	   return float4(1, 1, 1, 1)*20;

   return float4(finalColor, diffuse.a);
}