#ifndef _RAIN_H_
#define _RAIN_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <memory>
#include "CommonStates.h"
#include "textureclass.h"
#include "Effects.h"
#include "Model.h"
#include "d3dx11.h"
#include "DDSTextureLoader.h"
#include "../../../FX11-master/inc/d3dx11effect.h"
#include <vector>
#include <fstream>
#include <istream>
#include <d2d1.h>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <WICTextureLoader.h>

using namespace DirectX;
using namespace std;

extern float m_RainFactors[370];

class RainClass
{
private:

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT4 lightPosition;
	};

	/*
	struct SkyBufferType
	{
		XMMATRIX inverseView;
		XMMATRIX inverseProjection;
		XMFLOAT4 lightPosition;
	};
	*/

	struct SkyBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX WVP;
	};

	struct RainVertexType
	{
		XMFLOAT3 position;
		float    random;
		XMFLOAT3 seed;
		XMFLOAT3 speed;	
		unsigned int Type;
	};

	struct RainSoType
	{
		XMFLOAT4 position;
		XMFLOAT3 lightDir;
		XMFLOAT3 pointlightDir;
		XMFLOAT3 eyeVec;
		XMFLOAT2 tex;
		unsigned int type;
		float random;
	};

	struct SkyVertex
	{
		XMFLOAT3 pos;
	};

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
	};

	struct Light
	{
		XMFLOAT3 pos;
		float range;
		XMFLOAT3 dir;
		float cone;
		XMFLOAT3 att;
		float padding;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
	};

	struct ObjMatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT4 difColor;
		float    hasTexture;
		float    hasSpecular;
		XMFLOAT2 padding;
	};

	struct ObjLightBufferType
	{
		Light light;
	};

	struct SurfaceMaterial
	{
		wstring matName;
		XMFLOAT4 difColor;
		int texArrayIndex;
		bool hasTexture;
		bool transparent;
		bool specular;
	};

	struct splash
	{
		float timeCycle;
		float splashx;
		float splashy;
		float padding;
	};

public:
	RainClass();
	RainClass(const RainClass&);
	~RainClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND);
	void Shutdown();
	bool Render(ID3D11Device*,ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX,XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);

	float random(float a, float b);

private:
	HRESULT InitializeRainShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*);
	HRESULT InitializeRainAnimatedShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	HRESULT InitializeSkyShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	HRESULT InitializeObjShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	HRESULT InitializeDLightShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	HRESULT InitializeSLightShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetRainParameters(ID3D11Device*, ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);

	bool SetRainAnimatedParameters(ID3D11Device*, ID3D11DeviceContext*);

	bool SetSkyParameters(ID3D11Device*, ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);

	bool LoadTextureArray(ID3D11Device*, ID3D11DeviceContext*, char*, int, ID3D11Texture2D**, ID3D11ShaderResourceView**);

	bool loadLUTS(char*, int, int, ID3D11Device*);

	bool LoadObjModel(ID3D11Device*, std::wstring, ID3D11Buffer**, ID3D11Buffer**, vector<int>&, vector<int>&, vector<SurfaceMaterial>&, int&, vector<ID3D11ShaderResourceView*>&, bool, bool);

	bool Load3DTexture(ID3D11Device*, ID3D11DeviceContext*, WCHAR*, ID3D11Texture3D**, ID3D11ShaderResourceView**);

private:
	void RenderRainAnimated(ID3D11Device*, ID3D11DeviceContext*);
	void RenderRain(ID3D11DeviceContext*);
	void RenderSky(ID3D11DeviceContext*);
	void RenderObj(ID3D11Device*, ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);
	void RenderObjT(ID3D11Device*, ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);
	void RenderSLight(ID3D11Device*, ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);
	void RenderDLight(ID3D11Device*, ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);
	HRESULT CreateRainFactorsResource(ID3D11Device*);

private:
	//rain
	int m_vertexCount;
	ID3D11VertexShader*  m_vertexShader;
	ID3D11GeometryShader* m_geometryShader;
	ID3D11PixelShader*    m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_soBuffer;
	ID3D11ShaderResourceView* m_rainData = NULL;
	ID3D11SamplerState* m_SamplerPoint;
	ID3D11ShaderResourceView* m_rainFactorSRV;

	//rain animated
	bool firstFrame;
	ID3D11VertexShader*  m_vertexShaderAnimated;
	ID3D11GeometryShader* m_geometryStreamShader;
	ID3D11InputLayout* m_layout2;
	ID3D11Buffer* m_vertexBufferStart;
	ID3D11Buffer* m_vertexBufferDrawFrom;
	ID3D11Buffer* m_vertexBufferStreamTo;

	//sky
	ID3D11VertexShader*  m_skyvertexShader;
	ID3D11PixelShader*    m_skypixelShader;
	ID3D11InputLayout* m_skylayout;
	ID3D11Buffer* m_skyvertexBuffer;
	ID3D11Buffer* m_skyindexBuffer;
	ID3D11Buffer* m_skyconstantBuffer;
	ID3D11Buffer* m_skysoBuffer;
	ID3D11Texture2D* m_skytexture;
	ID3D11ShaderResourceView* m_skySRV;
	ID3D11SamplerState* m_skySamplerPoint;
	int m_numSphereFaces;
	
	//build
	ID3D11VertexShader* m_objvertexShader;
	ID3D11PixelShader* m_objpixelShader;
	ID3D11InputLayout* m_objlayout;
	ID3D11Buffer* m_objvertexBuffer;
	ID3D11Buffer* m_objindexBuffer;
	vector<int> meshSubsetIndexStart;
	vector<int> meshSubsetTexture;
	vector<SurfaceMaterial> material;
	int meshSubsets = 0;
	vector<ID3D11ShaderResourceView*> objmeshSRV;
	vector<wstring> textureNameArray;
	ID3D11BlendState* Transparency;
	ID3D11Buffer* m_objconstantBuffer1;
	ID3D11Buffer* m_objconstantBuffer2;
	ID3D11Buffer* m_objconstantBuffer3;
	ID3D11SamplerState* m_objSamplerPoint;
	ID3D11SamplerState* m_splashSamplerPoint;
	ID3D11ShaderResourceView* splashSRV = NULL;
	ID3D11ShaderResourceView* splashSRV2;
	float m_timeCycle = 0.0f;
	ID3D11ShaderResourceView* specularSRV;

	//street light
	ID3D11VertexShader* m_slightvertexShader;
	ID3D11PixelShader*  m_slightpixelShader;
	ID3D11InputLayout*  m_slightlayout;
	ID3D11Buffer* m_slightvertexBuffer;
	ID3D11Buffer* m_slightindexBuffer;
	vector<int> slightmeshSubsetIndexStart;
	vector<int> slightmeshSubsetTexture;
	vector<SurfaceMaterial> slightmaterial;
	int slightmeshSubsets = 0;
	vector<ID3D11ShaderResourceView*> slightmeshSRV;
	vector<wstring> slighttextureNameArray;
	ID3D11Buffer* m_slightconstantBuffer1;
	ID3D11Buffer* m_slightconstantBuffer2;
	ID3D11SamplerState* m_slightSamplerPoint;

	//direction light
	ID3D11VertexShader* m_dlightvertexShader;
	ID3D11PixelShader* m_dlightpixelShader;
	ID3D11InputLayout* m_dlightlayout;
	ID3D11Buffer* m_dlightconstantBuffer;
	ID3D11Buffer* m_dlightvertexBuffer;
	ID3D11Buffer* m_dlightindexBuffer;
	vector<int> dlightmeshSubsetIndexStart;
	vector<int> dlightmeshSubsetTexture;
	vector<SurfaceMaterial> dlightmaterial;
	int dlightmeshSubsets = 0;
	vector<ID3D11ShaderResourceView*> dlightmeshSRV;

	unique_ptr<CommonStates> m_states;
	unique_ptr<IEffectFactory> m_fxFactory;
	unique_ptr<Model> m_model;

	unique_ptr<CommonStates> m_states2;
	unique_ptr<IEffectFactory> m_fxFactory2;
	unique_ptr<Model> m_model2;
};
#endif

