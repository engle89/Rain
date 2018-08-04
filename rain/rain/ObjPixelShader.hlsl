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

cbuffer Splash : register(b2)
{
	float timeCycle;
	float splashx;
	float splashy;
	float padding2;
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

Texture3D SplashDiffuseTexture  : register(t1);
Texture3D SplashBumpTexture     : register(t2);
SamplerState SplashSamplerState : register(s1);

Texture2D ObjSpecular : register(t3);

float3 Tangent(float3 normal)
{
	float3 tangent;

	float3 c1 = cross(normal, float3(0.0, 0.0, 1.0));
	float3 c2 = cross(normal, float3(0.0, 1.0, 0.0));

	if (length(c1) > length(c2))
		tangent = c1;
	else
		tangent = c2;

	tangent = normalize(tangent);
	return tangent;
}

float3 Specular(float lightIntensity, float Ks, float Dsp, float Dvp, float specPow, float3 L, float3 VReflect)
{
	lightIntensity = lightIntensity * 100;
	float LDotVReflect = dot(L, VReflect);
	float thetas = acos(LDotVReflect);

	float t1 = exp(-0.04 * Dsp)*pow(max(LDotVReflect, 0), specPow) / Dsp;
	//float4 t2 = 0.04*G_20table.SampleLevel(samLinearClamp, float2((0.04*Dsp - g_20XOffset)*g_20XScale, (thetas - g_20YOffset)*g_20YScale), 0) / (2 * 3.1415);
	//float specular = (t1 + t2.x)*exp(-0.04*Dvp)*Ks*lightIntensity / Dsp;
	float specular = t1*exp(-0.04*Dvp)*Ks*lightIntensity / Dsp;
	return specular.xxx;
}

float3 Reflect(float3 N, float3 L)
{
	float3 I = -L;
	float3 R = I - 2 * (dot(N, I))*N;
	return R;
}

float4 ObjPixelShader(PSInput input) : SV_TARGET
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

	//splash
	/*
	float4 sceneSpecular = float4(0.0, 0.0, 0.0, 0.0);
	if (hasSpecular == 1)
		sceneSpecular = ObjSpecular.Sample(ObjSamplerState, input.Tex);

	float3 tangent = Tangent(input.Normal);
	float3 binorm = normalize(cross(input.Normal, tangent));
	float wetSurf = saturate(10 / 2.0*saturate(input.Normal.y));

	if (dot(normalize(input.worldPos.xyz), binorm) < 0)
		binorm = -binorm;
	float3x3 BTNMatrix = float3x3(binorm, tangent, input.Normal);
	float3 N = normalize(mul(input.Normal, BTNMatrix));

	float4 BumpMapVal = SplashBumpTexture.Sample(SplashSamplerState, float3(input.worldPos.x / 2.0 + splashx, input.worldPos.z / 2.0 + splashy, timeCycle)) - 0.5;
	N += wetSurf * 2 * (BumpMapVal.x*tangent + BumpMapVal.y*binorm);
	N = normalize(N);
	float3 splashDiffuse = wetSurf * SplashDiffuseTexture.Sample(SplashSamplerState, float3(input.worldPos.xz, timeCycle));

	float viewDir = input.worldPos;
	float Dvp = length(viewDir);
	viewDir = viewDir / Dvp;
	float3 reflec = Reflect(N, viewDir);
	float3 specular = Specular(1.0, 20, length(light.dir), Dvp, 20, normalize(light.dir), reflec);
	splashDiffuse += sceneSpecular.xyz;
	float3 s = float3(splashDiffuse.x*specular.x, splashDiffuse.y*specular.y, splashDiffuse.z*specular.z);
	*/

	finalColor = saturate(finalColor + finalAmbient);

	return float4(finalColor, diffuse.a);
}