#define PI 3.14159265

//constant buffer that can be updated every frame
cbuffer matrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 lightPosition;
};

struct PSInput
{
	float4 pos           : SV_POSITION;
	float3 lightDir      : LIGHT;
	float3 pointLightDir : LIGHT2;
	float3 eyeVec        : EYE;
	float2 tex           : TEX;
	float  random        : RAND;
	uint   type          : TYPE;
};

//generic sampler
SamplerState samAniso : register(s0);
//the texture containing the rain 
Texture2DArray<float> rainTexture : register(t0);
Texture1D<float> rainFactor: register(t1);

float RainFactorValue(int i)
{
	float factor;

	if (i >= 370)
		factor = -1.0f;
	else
		factor = rainFactor.Load(int2(i,0));

	return factor;
}

void rainResponse(PSInput input, float3 lightVector, float lightIntensity, float3 lightColor, float3 eyeVector, bool fallOffFactor, inout float4 rainResponseVal)
{
	float opacity = 0.0;

	float fallOff;
	if (fallOffFactor)
	{
		float disToLight = length(lightVector);
		fallOff = 1.0 / (disToLight*disToLight);
		fallOff = saturate(fallOff);
	}
	else
		fallOff = 1;

	if (fallOff > 0.01 && lightIntensity > 0.01)
	{
		float3 dropDir = float3(0, -0.35, 0.0);

        #define MAX_VIDX 4
        #define MAX_HIDX 8
		float3 L = normalize(lightVector);
		float3 E = normalize(eyeVector);
		float3 N = normalize(dropDir);

		bool is_EpLp_angle_ccw = true;
		float hangle = 0;
		float vangle = abs((acos(dot(L, N)) * 180 / PI) - 90);

		{
			float3 Lp = normalize(L - dot(L, N)*N);
			float3 Ep = normalize(E - dot(E, N)*N);
			hangle = acos(dot(Ep, Lp)) * 180 / PI;
			hangle = (hangle - 10) / 20.0;
			is_EpLp_angle_ccw = dot(N, cross(Ep, Lp)) > 0;
		}

		if (vangle >= 88.0)
		{
			hangle = 0;
			is_EpLp_angle_ccw = true;
		}

		vangle = (vangle - 10.0) / 20.0;

		int verticalLightIndex1 = floor(vangle);
		int verticalLightIndex2 = min(MAX_VIDX, (verticalLightIndex1 + 1));
		verticalLightIndex1 = max(0, verticalLightIndex1);
		float t = frac(vangle);

		float textureCoordsH1 = input.tex.x;
		float textureCoordsH2 = input.tex.x;

		int horizontalLightIndex1 = 0;
		int horizontalLightIndex2 = 0;
		float s = 0;

		s = frac(hangle);
		horizontalLightIndex1 = floor(hangle);
		horizontalLightIndex2 = horizontalLightIndex1 + 1;
		if (horizontalLightIndex1 < 0)
		{
			horizontalLightIndex1 = 0;
			horizontalLightIndex2 = 0;
		}

		if (is_EpLp_angle_ccw)
		{
			if (horizontalLightIndex2 > MAX_HIDX)
			{
				horizontalLightIndex2 = MAX_HIDX;
				textureCoordsH2 = 1.0 - textureCoordsH2;
			}
		}
		else
		{
			textureCoordsH1 = 1.0 - textureCoordsH1;
			if (horizontalLightIndex2 > MAX_HIDX)
				horizontalLightIndex2 = MAX_HIDX;
			else
				textureCoordsH2 = 1.0 - textureCoordsH2;
		}

		if (verticalLightIndex1 >= MAX_VIDX)
		{
			textureCoordsH2 = input.tex.x;
			horizontalLightIndex1 = 0;
			horizontalLightIndex2 = 0;
			s = 0;
		}

		//Generate the final texture coordinates for each sample
		uint type = input.type;
		uint2 texIndicesV1 = uint2(verticalLightIndex1 * 90 + horizontalLightIndex1 * 10 + type,
			                       verticalLightIndex1 * 90 + horizontalLightIndex2 * 10 + type);
		float3 tex1 = float3(textureCoordsH1, input.tex.y, texIndicesV1.x);
		float3 tex2 = float3(textureCoordsH2, input.tex.y, texIndicesV1.y);
		if ((verticalLightIndex1 < 4) && (verticalLightIndex2 >= 4))
		{
			s = 0;
			horizontalLightIndex1 = 0;
			horizontalLightIndex2 = 0;
			textureCoordsH1 = input.tex.x;
			textureCoordsH2 = input.tex.x;
		}

		uint2 texIndicesV2 = uint2(verticalLightIndex2 * 90 + horizontalLightIndex1 * 10 + type,
			                       verticalLightIndex2 * 90 + horizontalLightIndex2 * 10 + type);
		float3 tex3 = float3(textureCoordsH1, input.tex.y, texIndicesV2.x);
		float3 tex4 = float3(textureCoordsH2, input.tex.y, texIndicesV2.y);

		//Sample opacity from the textures
		float col1 = rainTexture.Sample(samAniso, tex1) * RainFactorValue(texIndicesV1.x);
		float col2 = rainTexture.Sample(samAniso, tex2) * RainFactorValue(texIndicesV1.y);
		float col3 = rainTexture.Sample(samAniso, tex3) * RainFactorValue(texIndicesV2.x);
		float col4 = rainTexture.Sample(samAniso, tex4) * RainFactorValue(texIndicesV2.y);

		//Compute interpolated opacity using the s and t factors
		float hOpacity1 = lerp(col1, col2, s);
		float hOpacity2 = lerp(col3, col4, s);
		opacity = lerp(hOpacity1, hOpacity2, t);
		opacity = pow(opacity, 0.7); // inverse gamma correction (expand dynamic range)
		opacity = 4 * lightIntensity * opacity * fallOff;
	}

	rainResponseVal = float4(lightColor, opacity);
}

float4 RainPixelShader(PSInput input) : SV_TARGET
{
	float4 directionalLight;
    rainResponse(input, input.lightDir, 2.0 * 0.25 * input.random, float3(1.0, 1.0, 1.0), input.eyeVec, false, directionalLight);

	float4 pointLight = float4(0, 0, 0, 0);
	float3 L = normalize(input.pointLightDir);
	float angleToSpotLight = dot(-L, float3(0, -1, 0));
	if (angleToSpotLight > 0.8)
		rainResponse(input, input.pointLightDir, 2.0 * 2.00 * input.random, float3(1.0, 1.0, 1.0), input.eyeVec, true, pointLight);

	float totalOpacity = pointLight.w + directionalLight.w;

	return float4(float3(pointLight.xyz*pointLight.w / totalOpacity + directionalLight.xyz*directionalLight.w / totalOpacity), totalOpacity);
}