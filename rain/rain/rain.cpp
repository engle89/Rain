#include "rain.h"

RainClass::RainClass()
{
	
}

RainClass::RainClass(const RainClass&)
{

}

RainClass::~RainClass()
{

}

float RainClass::random(float a, float b)
{
	return ((b - a)*((float)rand() / RAND_MAX)) + a;
}

bool RainClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	HRESULT result;

	//loading models
	//only able to use models less than around 60,000 vertices
	/*
	m_states = make_unique<CommonStates>(device);
	m_fxFactory = make_unique<EffectFactory>(device);
	WCHAR mesh[34] = L"D:/engle/rain/rain/rain/Build.cmo";
	m_model = Model::CreateFromCMO(device, mesh, *m_fxFactory);

	m_states2 = make_unique<CommonStates>(device);
	m_fxFactory2 = make_unique<EffectFactory>(device);
	WCHAR mesh2[40] = L"D:/engle/rain/rain/rain/street_lamp.cmo";
	m_model2 = Model::CreateFromCMO(device, mesh2, *m_fxFactory2);
	*/

	
	result = CreateRainFactorsResource(device);
	if (FAILED(result))
		return false;

	ID3D11Texture2D* rainTexture = NULL;
	char rain[52] = "D:/engle/rain/rain/rain/rainTextures/cv0_vPositive_";
	bool v = LoadTextureArray(device, deviceContext, rain, 370, &rainTexture, &m_rainData);
	if (v != true)
		return false;

	WCHAR vsFilename[46] = L"D:/engle/rain/rain/rain/RainVertexShader.hlsl";
	WCHAR gsFilename[48] = L"D:/engle/rain/rain/rain/RainGeometryShader.hlsl";
	WCHAR psFilename[45] = L"D:/engle/rain/rain/rain/RainPixelShader.hlsl";
	result = InitializeRainShader(device, hwnd, vsFilename, gsFilename, psFilename);
	if (FAILED(result))
		return false;

	char Ftable[39] = "D:/engle/rain/rain/rain/F_512_data.csv";
	v = loadLUTS(Ftable, 512, 512, device);
	if (v != true)
		return false;

	WCHAR vsRainAnimated[54] = L"D:/engle/rain/rain/rain/RainAnimatedVertexShader.hlsl";
	WCHAR gsRainAnimated[56] = L"D:/engle/rain/rain/rain/RainAnimatedGeometryShader.hlsl";
	result = InitializeRainAnimatedShader(device, hwnd, vsRainAnimated, gsRainAnimated);
	if (FAILED(result))
		return false;

	WCHAR vsSkyName[45] = L"D:/engle/rain/rain/rain/SkyVertexShader.hlsl";
	WCHAR psSkyName[44] = L"D:/engle/rain/rain/rain/SkyPixelShader.hlsl";
	result = InitializeSkyShader(device, hwnd, vsSkyName, psSkyName);
	if (FAILED(result))
		return false;

	WCHAR vsObjName[45] = L"D:/engle/rain/rain/rain/ObjVertexShader.hlsl";
	WCHAR psObjName[44] = L"D:/engle/rain/rain/rain/ObjPixelShader.hlsl";
	result = InitializeObjShader(device, hwnd, vsObjName, psObjName);
	if (FAILED(result))
		return false;
	WCHAR obj[34] = L"D:/engle/rain/rain/rain/Build.obj";
	v = LoadObjModel(device,obj, &m_objvertexBuffer, &m_objindexBuffer, meshSubsetIndexStart, meshSubsetTexture, material, meshSubsets, objmeshSRV,false, false);
	if (v != true)
		return false;

	WCHAR splashDiffuse[43] = L"D:/engle/rain/rain/rain/SDiffuseVolume.dds";
	ID3D11Texture3D* splashDiffuseTexture = NULL;
	v = Load3DTexture(device, deviceContext, splashDiffuse, &splashDiffuseTexture, &splashSRV);
	if (v != true)
		return false;

    WCHAR splashBump[40] = L"D:/engle/rain/rain/rain/SBumpVolume.dds";
	ID3D11Texture3D* splashBumpTexture = NULL;
	v = Load3DTexture(device, deviceContext, splashBump, &splashBumpTexture, &splashSRV2);
	if (v != true)
		return false;

	wstring specular = L"specular.jpg";
	ID3D11Resource* tempTexture;
	result = CreateWICTextureFromFile(device,
		                              specular.c_str(),
		                              &tempTexture,
		                              &specularSRV,
		                              0);
	if (FAILED(result))
		return false;

	WCHAR vsSLightName[48] = L"D:/engle/rain/rain/rain/SLightVertexShader.hlsl";
	WCHAR psSLightName[47] = L"D:/engle/rain/rain/rain/SLightPixelShader.hlsl";
	result = InitializeSLightShader(device, hwnd, vsSLightName, psSLightName);
	if (FAILED(result))
		return false;
	WCHAR slight[40] = L"D:/engle/rain/rain/rain/street_lamp.obj";
	v = LoadObjModel(device, slight, &m_slightvertexBuffer, &m_slightindexBuffer, slightmeshSubsetIndexStart, slightmeshSubsetTexture, slightmaterial, slightmeshSubsets, slightmeshSRV,false, true);
	if (v != true)
		return false;

	/*
	WCHAR vsDLightName[48] = L"D:/engle/rain/rain/rain/DLightVertexShader.hlsl";
	WCHAR psDLightName[47] = L"D:/engle/rain/rain/rain/DLightPixelShader.hlsl";
	result = InitializeDLightShader(device, hwnd, vsDLightName, psDLightName);
	if (FAILED(result))
		return false;
	WCHAR dlight[35] = L"D:/engle/rain/rain/rain/arrow4.obj";
	v = LoadObjModel(device, dlight, &m_dlightvertexBuffer, &m_dlightindexBuffer, dlightmeshSubsetIndexStart, dlightmeshSubsetTexture, dlightmaterial, dlightmeshSubsets, dlightmeshSRV,false, true);
	if (v != true)
		return false;
		*/
	
	return true;
}

void RainClass::Shutdown()
{
	ShutdownShader();
	return;
}

bool RainClass::Render(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX worldMatrix2, XMMATRIX worldMatrix3, XMMATRIX worldMatrix4, XMMATRIX worldMatrix5, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	bool result;

	//m_model->Draw(deviceContext, *m_states, worldMatrix, viewMatrix, projectionMatrix);

	//m_model2->Draw(deviceContext, *m_states2, worldMatrix2, viewMatrix, projectionMatrix);

	result = SetSkyParameters(device, deviceContext, worldMatrix2, viewMatrix, projectionMatrix, lightPosition);
	if (!result)
		return false;
	RenderSky(deviceContext);
	
	RenderObj(device, deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightPosition);

	RenderSLight(device, deviceContext, worldMatrix5, viewMatrix, projectionMatrix, lightPosition);

	//RenderDLight(device, deviceContext, worldMatrix4, viewMatrix, projectionMatrix, lightPosition);

	result = SetRainAnimatedParameters(device, deviceContext);
	if (!result)
		return false;
	RenderRainAnimated(device, deviceContext);

	result = SetRainParameters(device, deviceContext, worldMatrix3, viewMatrix, projectionMatrix, lightPosition);
	if (!result)
		return false;
    RenderRain(deviceContext);

	return true;
}

HRESULT RainClass::InitializeRainShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* geometryShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC soBufferDesc;

	//compile shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "RainVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(gsFilename, NULL, NULL, "RainGeometryShader", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &geometryShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, gsFilename);
		else
			MessageBox(hwnd, gsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "RainPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;
	
	/*
	//stream outputstage input signature declaration
	D3D11_SO_DECLARATION_ENTRY decl[] =
	{
	    { 0, "SV_POSITION", 0, 0, 4, 0 },
	    { 0, "LIGHT",       0, 0, 3, 0 },
	    { 0, "LIGHT",       2, 0, 3, 0 },
	    { 0, "EYE",         0, 0, 3, 0 },
	    { 0, "TEX",         0, 0, 2, 0 },
	    { 0, "TYPE",        0, 0, 1, 0 },
	    { 0, "RAND",        0, 0, 1, 0 }
	};
	UINT stream = (UINT)0;
	result = device->CreateGeometryShaderWithStreamOutput(
		geometryShaderBuffer->GetBufferPointer(),
		geometryShaderBuffer->GetBufferSize(),
		decl, //so declaration
		(UINT)7, //num of entries
		NULL,
		0,
		stream,
		NULL,
		&m_geometryShader);
	if (FAILED(result))
		return false;
	*/

	result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;


	//create the vertex input layout for rain
	const D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    {"RAND",     0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    {"SEED",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    {"SPEED",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},	   
	    {"TYPE",     0, DXGI_FORMAT_R8_UINT,         0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	numElements = sizeof(layout) / sizeof(layout[0]);
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//constant buffer
	//matrix buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	//SO stage buffer
	ZeroMemory(&soBufferDesc, sizeof(soBufferDesc));
	soBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	soBufferDesc.ByteWidth = m_vertexCount * sizeof(RainSoType);
	soBufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	soBufferDesc.CPUAccessFlags = 0;
	soBufferDesc.MiscFlags = 0;
	soBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&soBufferDesc, NULL, &m_soBuffer);
	if (FAILED(result))
		return false;

	//create a point sampler for rain
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = 0;
	result = device->CreateSamplerState(&sampleDesc, &m_SamplerPoint);
	if (FAILED(result))
		return false;

	return result;
}

HRESULT RainClass::InitializeRainAnimatedShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* gsFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* geometryShaderBuffer = 0;
	unsigned int numElements;
	RainVertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc;

	//compile shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "RainAnimatedVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "RainAnimatedVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &geometryShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShaderAnimated);
	if (FAILED(result))
		return false;

	//stream outputstage input signature declaration
	D3D11_SO_DECLARATION_ENTRY decl[] =
	{
	{ 0, "POSITION",   0, 0, 3, 0 },
	{ 0, "RAND",       0, 0, 1, 0 },
	{ 0, "SEED",       0, 0, 3, 0 },
	{ 0, "SPEED",      0, 0, 3, 0 },	
	{ 0, "TYPE",       0, 0, 1, 0 }
	};
	UINT stream = (UINT)0;
	result = device->CreateGeometryShaderWithStreamOutput(
	geometryShaderBuffer->GetBufferPointer(),
	geometryShaderBuffer->GetBufferSize(),
	decl, //so declaration
	(UINT)5, //num of entries
	NULL,
	0,
	stream,
	NULL,
	&m_geometryStreamShader);
	if (FAILED(result))
	return false;
	

	//set up shader buffers
	//vertex buffer
	firstFrame = true;
	m_vertexCount = 150000;
	vertices = new RainVertexType[m_vertexCount];
	for (int i = 0; i < m_vertexCount; i++)
	{
		RainVertexType raindrop;

		//generate random points inside a circle of raduis 1 centered at 0,0
		float SeedX;
		float SeedZ;
		bool pointIsInside = false;
		while (!pointIsInside)
		{
			SeedX = random(0, 1) - 0.5f;
			SeedZ = random(0, 1) - 0.5f;
			if (sqrt(SeedX*SeedX + SeedZ * SeedZ) <= 0.5f)
				pointIsInside = true;
		}

		//save these random locations for reinitializing rain particles that have fallen out of bounds
		SeedX *= 45.0f;
		SeedZ *= 45.0f;
		float SeedY = random(0, 1) * 40.f;
		raindrop.seed = XMFLOAT3(SeedX, SeedY, SeedZ);

		//add some random speed
		float SpeedX = 40.0f*(random(0, 1) / 20.0f);
		float SpeedZ = 40.0f*(random(0, 1) / 20.0f);
		float SpeedY = 40.0f*(random(0, 1) / 10.0f);
		raindrop.speed = XMFLOAT3(SpeedX, SpeedY, SpeedZ);

		//move the rain particles to a random positions in a cylinder above the camera
		float x = SeedX;
		float z = SeedZ;
		float y = SeedY;
		raindrop.position = XMFLOAT3(x, y, z);

		//get an integer between 1 and 8 inclusive to decide which of the 8 types of rain textures the particle will use
		raindrop.Type = int(floor(random(0, 1) * 8 + 1));

		//this number is used to randomly increase the brightness of some rain particles
		raindrop.random = 1;
		float randomIncrease = random(0, 1);
		if (randomIncrease > 0.8)
			raindrop.random += randomIncrease;

		vertices[i] = raindrop;
	}

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(RainVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//vertex buffer initial data description
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	initData.SysMemPitch = sizeof(RainVertexType);
	result = device->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBufferStart);
	if (FAILED(result))
		return false;
	vertexBufferDesc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	vertexBufferDesc.ByteWidth = sizeof(RainVertexType)*m_vertexCount;
	result = device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBufferDrawFrom);
	if (FAILED(result))
		return false;
	result = device->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBufferStreamTo);
	if (FAILED(result))
		return false;

	//create the vertex input layout for rain
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "RAND",     0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "SEED",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "SPEED",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   
	    { "TYPE",     0, DXGI_FORMAT_R8_UINT,         0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = sizeof(layout) / sizeof(layout[0]);
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout2);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;

	return result;
}

HRESULT RainClass::InitializeSkyShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	unsigned int numElements;

	//compile shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "SkyVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "SkyPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_skyvertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_skypixelShader);
	if (FAILED(result))
		return false;

	/*
	//set up shader buffers
	//vertex buffer
	//create the vertex input layout for rain
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = sizeof(layout) / sizeof(layout[0]);
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_skylayout);
	if (FAILED(result))
		return false;

	
	XMFLOAT2 vertices[4];
	vertices[0] = XMFLOAT2(1.0f,   1.0f);
	vertices[1] = XMFLOAT2(1.0f,  -1.0f);
	vertices[2] = XMFLOAT2(-1.0f,  1.0f);
	vertices[3] = XMFLOAT2(-1.0f, -1.0f);

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(XMFLOAT2) * 4;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	result = device->CreateBuffer(&desc, &InitData, &m_skyvertexBuffer);

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//constant buffer
	D3D11_BUFFER_DESC cdesc;
	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(SkyBufferType);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_skyconstantBuffer);
	if (FAILED(result))
		return false;

	//create a point sampler for sky
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = 0;
	result = device->CreateSamplerState(&sampleDesc, &m_skySamplerPoint);
	if (FAILED(result))
		return false;

	return result;
	*/
	//set up shader buffers
	//vertex buffer
	//create the vertex input layout for rain
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = sizeof(layout) / sizeof(layout[0]);
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_skylayout);
	if (FAILED(result))
		return false;

	//create a sphere to map our skycube map
	unsigned int LatLines = 10;
	unsigned int LongLines = 10;
	int NUmSphereVertices = ((LatLines - 2)*LongLines) + 2;
	int NumSphereFaces = ((LatLines - 3)*LongLines * 2) + (LongLines * 2);
	m_numSphereFaces = NumSphereFaces;

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	vector<SkyVertex> vertices(NUmSphereVertices);
	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 0.0f;
	vertices[0].pos.z = 1.0f;

	for (DWORD i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1)*(3.14f / (LatLines - 1));
		XMMATRIX Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / LongLines);
			XMMATRIX Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx*Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i*LongLines + j + 1].pos.x = XMVectorGetX(currVertPos);
			vertices[i*LongLines + j + 1].pos.y = XMVectorGetY(currVertPos);
			vertices[i*LongLines + j + 1].pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[NUmSphereVertices - 1].pos.x = 0.0f;
	vertices[NUmSphereVertices - 1].pos.y = 0.0f;
	vertices[NUmSphereVertices - 1].pos.z = -1.0f;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SkyVertex)*NUmSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	result = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_skyvertexBuffer);

	vector<DWORD> indices(NumSphereFaces * 3);
	int k = 0;
	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < LatLines - 3; ++i)
	{
		for (DWORD j = 0; j < LongLines - 1; ++j)
		{
			indices[k  ]   = i       * LongLines + j + 1;
			indices[k+1]   = i       * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;

			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i       * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;

			k += 6; //next quad
		}

		indices[k    ] = i * LongLines + LongLines;
		indices[k + 1] = i * LongLines + 1;
		indices[k + 2] = (i + 1) * LongLines + LongLines;

		indices[k + 3] = (i + 1) * LongLines + LongLines;
		indices[k + 4] = i       * LongLines + 1;
		indices[k + 5] = (i + 1) * LongLines + LongLines;

		k += 6; 
	}

	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k]     = NUmSphereVertices - 1;
		indices[k + 1] = (NUmSphereVertices - 1) - (l + 1);
		indices[k + 2] = NUmSphereVertices - 2;
		k += 3;
	}

	indices[k] = NUmSphereVertices - 1;
	indices[k + 1] = (NUmSphereVertices - 1) - LongLines;
	indices[k + 2] = NUmSphereVertices - 1;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD)*NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	device->CreateBuffer(&indexBufferDesc, &iinitData, &m_skyindexBuffer);

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//constant buffer
	D3D11_BUFFER_DESC cdesc;
	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(SkyBufferType);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_skyconstantBuffer);
	if (FAILED(result))
		return false;

	//create a point sampler for sky
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&sampleDesc, &m_skySamplerPoint);
	if (FAILED(result))
		return false;

	//texture
	vector <wstring> faces;
	faces.push_back(L"D:/engle/rain/rain/rain/starfield_rt.jpg");
	faces.push_back(L"D:/engle/rain/rain/rain/starfield_lf.jpg");
	faces.push_back(L"D:/engle/rain/rain/rain/starfield_up.jpg");
	faces.push_back(L"D:/engle/rain/rain/rain/starfield_dn.jpg");
	faces.push_back(L"D:/engle/rain/rain/rain/starfield_ft.jpg");
	faces.push_back(L"D:/engle/rain/rain/rain/starfield_bk.jpg");

	ID3D11Resource* srcTex[6];
	ID3D11ShaderResourceView* tempMeshSRV;
	for (unsigned int i = 0; i < faces.size(); ++i)
	{
		result = CreateWICTextureFromFile(device,
			                              faces[i].c_str(),
			                              &srcTex[i],
			                              &tempMeshSRV,
			                              0);
		if (FAILED(result))
			return false;
	}

	D3D11_TEXTURE2D_DESC texElementDesc;
	((ID3D11Texture2D*)srcTex[0])->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = 6;
	texArrayDesc.Format             = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* texArray = 0;
	result = device->CreateTexture2D(&texArrayDesc, 0, &texArray);
	if (FAILED(result))
		return false;

	ID3D11DeviceContext* deviceContext;
	device->GetImmediateContext(&deviceContext);
	D3D11_BOX sourceRegion;

	for (UINT x = 0; x < 6; x++)
	{
		for (UINT mipLevel = 0; mipLevel < texArrayDesc.MipLevels; mipLevel++)
		{
			sourceRegion.left = 0;
			sourceRegion.right = (texArrayDesc.Width >> mipLevel);
			sourceRegion.top = 0;
			sourceRegion.bottom = (texArrayDesc.Height >> mipLevel);
			sourceRegion.front = 0;
			sourceRegion.back = 1;

			//test for overflow
			if (sourceRegion.bottom == 0 || sourceRegion.right == 0)
				break;

			deviceContext->CopySubresourceRegion(texArray, D3D11CalcSubresource(mipLevel, x, texArrayDesc.MipLevels), 0, 0, 0, srcTex[x], mipLevel, &sourceRegion);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.TextureCube.MostDetailedMip = 0;
	viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;
	result = device->CreateShaderResourceView(texArray, &viewDesc, &m_skySRV);
	if (FAILED(result))
		return false;

	return result;
}

HRESULT RainClass::InitializeObjShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	unsigned int numElements;

	//compile shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ObjVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "ObjPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_objvertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_objpixelShader);
	if (FAILED(result))
		return false;

	//set up shader buffers
	//vertex buffer
	//create the vertex input layout for rain
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,      0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = sizeof(layout) / sizeof(layout[0]);
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_objlayout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//constant buffer
	D3D11_BUFFER_DESC cdesc;
	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(ObjMatrixBufferType);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_objconstantBuffer1);
	if (FAILED(result))
		return false;

	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(Light);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_objconstantBuffer2);
	if (FAILED(result))
		return false;

	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(splash);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_objconstantBuffer3);
	if (FAILED(result))
		return false;

	//create a point sampler for obj
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&sampleDesc, &m_objSamplerPoint);
	if (FAILED(result))
		return false;

	//create a mirror sampler for splash
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&sampleDesc, &m_splashSamplerPoint);
	if (FAILED(result))
		return false;

	return result;
}

HRESULT RainClass::InitializeDLightShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	unsigned int numElements;

	//compile shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "DLightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "DLightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_dlightvertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_dlightpixelShader);
	if (FAILED(result))
		return false;

	//set up shader buffers
	//vertex buffer
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = sizeof(layout) / sizeof(layout[0]);
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_dlightlayout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//constant buffer
	D3D11_BUFFER_DESC cdesc;
	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(MatrixBufferType);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_dlightconstantBuffer);
	if (FAILED(result))
		return false;

	return result;
}

HRESULT RainClass::InitializeSLightShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	unsigned int numElements;

	//compile shaders
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "SLightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "SLightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		else
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_slightvertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_slightpixelShader);
	if (FAILED(result))
		return false;

	//set up shader buffers
	//vertex buffer
	//create the vertex input layout for rain
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,      0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = sizeof(layout) / sizeof(layout[0]);
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_slightlayout);
	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//constant buffer
	D3D11_BUFFER_DESC cdesc;
	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(ObjMatrixBufferType);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_slightconstantBuffer1);
	if (FAILED(result))
		return false;

	ZeroMemory(&cdesc, sizeof(cdesc));
	cdesc.Usage = D3D11_USAGE_DEFAULT;
	cdesc.ByteWidth = sizeof(Light);
	cdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdesc.CPUAccessFlags = 0;
	cdesc.MiscFlags = 0;
	cdesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cdesc, NULL, &m_slightconstantBuffer2);
	if (FAILED(result))
		return false;

	//create a point sampler for obj
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&sampleDesc, &m_slightSamplerPoint);
	if (FAILED(result))
		return false;

	return result;
}

void RainClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = 0;
	}

	if (m_SamplerPoint)
	{
		m_SamplerPoint->Release();
		m_SamplerPoint = 0;
	}

	if (m_rainData)
	{
		m_rainData->Release();
		m_rainData = 0;
	}
	
	if (m_skyconstantBuffer)
	{
		m_skyconstantBuffer->Release();
		m_skyconstantBuffer = 0;
	}

	if (m_skylayout)
	{
		m_skylayout->Release();
		m_skylayout = 0;
	}

	if (m_skypixelShader)
	{
		m_skypixelShader->Release();
		m_skypixelShader = 0;
	}

	if (m_skyvertexShader)
	{
		m_skyvertexShader->Release();
		m_skyvertexShader = 0;
	}

	if (m_skySamplerPoint)
	{
		m_skySamplerPoint->Release();
		m_skySamplerPoint = 0;
	}

	
	if (m_objconstantBuffer1)
	{
		m_objconstantBuffer1->Release();
		m_objconstantBuffer1 = 0;
	}

	if (m_objconstantBuffer2)
	{
		m_objconstantBuffer2->Release();
		m_objconstantBuffer2 = 0;
	}

	if (m_objlayout)
	{
		m_objlayout->Release();
		m_objlayout = 0;
	}

	if (m_objpixelShader)
	{
		m_objpixelShader->Release();
		m_objpixelShader = 0;
	}

	if (m_objvertexShader)
	{
		m_objvertexShader->Release();
		m_objvertexShader = 0;
	}

	if (m_objSamplerPoint)
	{
		m_objSamplerPoint->Release();
		m_objSamplerPoint = 0;
	}

	if (m_objvertexBuffer)
	{
		m_objvertexBuffer->Release();
		m_objvertexBuffer = 0;
	}

	if (m_objindexBuffer)
	{
		m_objindexBuffer->Release();
		m_objindexBuffer = 0;
	}
	/*
	if (m_dlightconstantBuffer)
	{
		m_dlightconstantBuffer->Release();
		m_dlightconstantBuffer = 0;
	}

	if (m_dlightlayout)
	{
		m_dlightlayout->Release();
		m_dlightlayout = 0;
	}

	if (m_dlightpixelShader)
	{
		m_dlightpixelShader->Release();
		m_dlightpixelShader = 0;
	}

	if (m_dlightvertexShader)
	{
		m_dlightvertexShader->Release();
		m_dlightvertexShader = 0;
	}

	if (m_dlightvertexBuffer)
	{
		m_dlightvertexBuffer->Release();
		m_dlightvertexBuffer = 0;
	}

	if (m_dlightindexBuffer)
	{
		m_dlightindexBuffer->Release();
		m_dlightindexBuffer = 0;
	}
	*/

	if (m_slightconstantBuffer1)
	{
		m_slightconstantBuffer1->Release();
		m_slightconstantBuffer1 = 0;
	}

	if (m_slightconstantBuffer2)
	{
		m_slightconstantBuffer2->Release();
		m_slightconstantBuffer2 = 0;
	}

	if (m_slightlayout)
	{
		m_slightlayout->Release();
		m_slightlayout = 0;
	}

	if (m_slightpixelShader)
	{
		m_slightpixelShader->Release();
		m_slightpixelShader = 0;
	}

	if (m_slightvertexShader)
	{
		m_slightvertexShader->Release();
		m_slightvertexShader = 0;
	}

	if (m_slightSamplerPoint)
	{
		m_slightSamplerPoint->Release();
		m_slightSamplerPoint = 0;
	}

	if (m_slightvertexBuffer)
	{
		m_slightvertexBuffer->Release();
		m_slightvertexBuffer = 0;
	}

	if (m_slightindexBuffer)
	{
		m_slightindexBuffer->Release();
		m_slightindexBuffer = 0;
	}

	return;
}

void RainClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i<bufferSize; i++)
		fout << compileErrors[i];

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool RainClass::SetRainParameters(ID3D11Device* device,ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{	
	//rain	

	//depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0x00;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* depthStencilState;
	HRESULT result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
		return false;

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	//rasterizer
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* rasterState;
	device->CreateRasterizerState(&rasterDesc, &rasterState);
	deviceContext->RSSetState(rasterState);

	//blending
	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = 0x0F;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	ID3D11BlendState* blendState;
	device->CreateBlendState(&blendDesc, &blendState);

	//Set our Blend State
	deviceContext->OMSetBlendState(blendState, NULL, 0xffffffff);

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
		
	//write data to constant matrix buffer
	MatrixBufferType cb;
	cb.world = worldMatrix;
	cb.view = viewMatrix;
	cb.projection = projectionMatrix;
	cb.lightPosition = lightPosition;
	deviceContext->UpdateSubresource(m_matrixBuffer, 0, NULL, &cb, 0, 0);

	//Finanly set the matrix constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	//geomtry shader
	deviceContext->GSSetConstantBuffers(0, 1, &m_matrixBuffer);
	
	//pixel shader
	deviceContext->PSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->PSSetSamplers(0, 1, &m_SamplerPoint);
	deviceContext->PSSetShaderResources(0, 1, &m_rainData);
	deviceContext->PSSetShaderResources(1, 1, &m_rainFactorSRV);		

	return true;
}

bool RainClass::SetRainAnimatedParameters(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	//depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0x00;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* depthStencilState;
	HRESULT result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
		return false;

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	return true;
}

bool RainClass::SetSkyParameters(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	/*
	//sky
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	XMMATRIX inverseProjectionMatrix = XMMatrixInverse(nullptr, projectionMatrix);
	XMMATRIX inverseViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
	XMMATRIX inverseWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);

	SkyBufferType sb;
	sb.inverseProjection = inverseProjectionMatrix;
	sb.inverseView = inverseViewMatrix;
	sb.lightPosition = lightPosition;
	deviceContext->UpdateSubresource(m_skyconstantBuffer, 0, NULL, &sb, 0, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &m_skyconstantBuffer);

	//deviceContext->GSSetConstantBuffers(0, 1, &m_skyconstantBuffer);

	deviceContext->PSSetConstantBuffers(0, 1, &m_skyconstantBuffer);
	deviceContext->PSSetSamplers(0, 1, &m_skySamplerPoint);
	deviceContext->PSSetShaderResources(0, 1, &m_skySRV);

	return true;
	*/
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* depthStencilState;
	HRESULT result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
		return false;

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* rasterState;
	device->CreateRasterizerState(&rasterDesc, &rasterState);
	deviceContext->RSSetState(rasterState);

	D3D11_BLEND_DESC blendStateDescription;
	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	ID3D11BlendState* BlendingState;
	result = device->CreateBlendState(&blendStateDescription, &BlendingState);
	if (FAILED(result))
		return false;
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	deviceContext->OMSetBlendState(BlendingState, blendFactor, 0xffffffff);


	worldMatrix          = XMMatrixTranspose(worldMatrix);
	viewMatrix           = XMMatrixTranspose(viewMatrix);
	projectionMatrix     = XMMatrixTranspose(projectionMatrix);
	XMMATRIX WVP         = XMMatrixMultiply(worldMatrix, viewMatrix);
	WVP                  = XMMatrixMultiply(WVP, projectionMatrix);

	SkyBufferType sb;
	sb.world         = worldMatrix;
	sb.view          = viewMatrix;
	sb.projection    = projectionMatrix;
	sb.WVP           = WVP;
	deviceContext->UpdateSubresource(m_skyconstantBuffer, 0, NULL, &sb, 0, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &m_skyconstantBuffer);

	//deviceContext->GSSetConstantBuffers(0, 1, &m_skyconstantBuffer);

	//deviceContext->PSSetConstantBuffers(0, 1, &m_skyconstantBuffer);
	deviceContext->PSSetSamplers(0, 1, &m_skySamplerPoint);
	deviceContext->PSSetShaderResources(0, 1, &m_skySRV);

	return true;
}

void RainClass::RenderRain(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	deviceContext->IASetInputLayout(m_layout);

	//Set the vertex buffer to use
	UINT stride = sizeof(RainVertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBufferDrawFrom, &stride, &offset);

	//Enable vertex shader
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);

	//Enable geometry shader
	deviceContext->GSSetShader(m_geometryShader, NULL, 0);

	//Enable pixel shader
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//Render the data
	deviceContext->Draw(m_vertexCount*0.7, 0);

	return;
}

void RainClass::RenderRainAnimated(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	//rain	
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_layout2);

	//Enable vertex shader
	deviceContext->VSSetShader(m_vertexShaderAnimated, NULL, 0);

	//Enable geometry shader
	deviceContext->GSSetShader(m_geometryStreamShader, NULL, 0);

	//Enable pixel shader
	deviceContext->PSSetShader(NULL, NULL, 0);

	//Set the vertex buffer to use
	ID3D11Buffer* pBuffers[1];
	if (firstFrame)
		pBuffers[0] = m_vertexBufferStart;
	else
		pBuffers[0] = m_vertexBufferDrawFrom;
	UINT stride = sizeof(RainVertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, pBuffers, &stride, &offset);
	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);

	//Set the SO buffer to use
	UINT soffset[1] = { 0 };
	pBuffers[0] = m_vertexBufferStreamTo;
	deviceContext->SOSetTargets(1, pBuffers, soffset);

	//Render the data
	deviceContext->Draw(m_vertexCount, 0);

	//Get back to normal
	pBuffers[0] = NULL;
	deviceContext->SOSetTargets(1, pBuffers, soffset);

	//Swap buffers
	ID3D11Buffer* temp     = m_vertexBufferDrawFrom;
	m_vertexBufferDrawFrom = m_vertexBufferStreamTo;
	m_vertexBufferStreamTo = temp;
	
	firstFrame = false;	
	
	return;
}

void RainClass::RenderSky(ID3D11DeviceContext* deviceContext)
{
	/*
	//sky
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_skylayout);

	//Set the vertex buffer to use
	UINT stride = sizeof(XMFLOAT2);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_skyvertexBuffer, &stride, &offset);

	//Enable vertex shader
	deviceContext->VSSetShader(m_skyvertexShader, NULL, 0);

	//Enable geometry shader
	deviceContext->GSSetShader(NULL, NULL, 0);

	//Enable pixel shader
	deviceContext->PSSetShader(m_skypixelShader, NULL, 0);

	//Render the data
	deviceContext->Draw(4, 0);

	return;
	*/

	//sky
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Set the vertex input layout
	deviceContext->IASetInputLayout(m_skylayout);

	//Set the index buffer to use
	deviceContext->IASetIndexBuffer(m_skyindexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the vertex buffer to use
	UINT stride = sizeof(SkyVertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_skyvertexBuffer, &stride, &offset);

	//Enable vertex shader
	deviceContext->VSSetShader(m_skyvertexShader, NULL, 0);

	//Enable geometry shader
	deviceContext->GSSetShader(NULL, NULL, 0);

	//Enable pixel shader
	deviceContext->PSSetShader(m_skypixelShader, NULL, 0);

	//Render the data
	deviceContext->DrawIndexed(m_numSphereFaces * 3, 0, 0);

	return;
}

void RainClass::RenderObj(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0x00;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* depthStencilState;
	HRESULT result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* rasterState;
	device->CreateRasterizerState(&rasterDesc, &rasterState);
	deviceContext->RSSetState(rasterState);

	D3D11_BLEND_DESC blendStateDescription;
	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	ID3D11BlendState* BlendingState;
	result = device->CreateBlendState(&blendStateDescription, &BlendingState);
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	deviceContext->OMSetBlendState(BlendingState, blendFactor, 0xffffffff);

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//Set shaders
	deviceContext->VSSetShader(m_objvertexShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(m_objpixelShader, NULL , 0);

	//light
	ObjLightBufferType cbl;
	cbl.light.pos = XMFLOAT3(lightPosition.x, lightPosition.y, lightPosition.z);
	//cbl.light.pos = XMFLOAT3(256.0f, 256.0f, 256.0f);
	cbl.light.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
	cbl.light.range = 20.0f;
	cbl.light.cone = 20.0f;
	//cbl.light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	cbl.light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	cbl.light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	cbl.light.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cbl.light.padding = 0.0f;
	deviceContext->UpdateSubresource(m_objconstantBuffer2, 0, NULL, &cbl, 0, 0);

	//splash
	ObjLightBufferType cb3;
	splash sp;
	m_timeCycle += 0.085;
	if (m_timeCycle >= 1)
	{
		m_timeCycle = 0;
		sp.splashx = random(0,1) * 2;
		sp.splashy = random(0,1) * 2;
	}
	else
	{
		sp.splashx = 0.0;
		sp.splashy = 0.0;
	}
	sp.timeCycle = m_timeCycle;
	sp.padding = 0;
	deviceContext->UpdateSubresource(m_objconstantBuffer3, 0, NULL, &cb3, 0, 0);

	//Nontransparent subsets
	for (int i = 0; i < meshSubsets; i++)
	{
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        deviceContext->IASetIndexBuffer(m_objindexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Set the vertex input layout
		deviceContext->IASetInputLayout(m_objlayout);

		//Set the vertex buffer to use
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_objvertexBuffer, &stride, &offset);

		//Set the constant buffer
		ObjMatrixBufferType cb;
		cb.world = worldMatrix;
		cb.view = viewMatrix;
		cb.projection = projectionMatrix;
		cb.difColor = material[meshSubsetTexture[i]].difColor;
		cb.hasTexture = material[meshSubsetTexture[i]].hasTexture;
		if (material[meshSubsetTexture[i]].specular)
			cb.hasSpecular = 1;
		else
			cb.hasSpecular = 0;
		cb.padding = XMFLOAT2(0.0f, 0.0f);
		deviceContext->UpdateSubresource(m_objconstantBuffer1, 0, NULL, &cb, 0, 0);

		//VS
		deviceContext->VSSetConstantBuffers(0, 1, &m_objconstantBuffer1);

		//PS
		deviceContext->PSSetConstantBuffers(0, 1, &m_objconstantBuffer1);
		deviceContext->PSSetConstantBuffers(1, 1, &m_objconstantBuffer2);
		deviceContext->PSSetConstantBuffers(2, 1, &m_objconstantBuffer3);
		if (material[meshSubsetTexture[i]].hasTexture)
		   deviceContext->PSSetShaderResources(0, 1, &objmeshSRV[material[meshSubsetTexture[i]].texArrayIndex]);
		if(material[meshSubsetTexture[i]].specular)
			deviceContext->PSSetShaderResources(3, 1, &specularSRV);
		deviceContext->PSSetSamplers(0, 1, &m_objSamplerPoint);
		deviceContext->PSSetShaderResources(1, 1, &splashSRV);
		deviceContext->PSSetShaderResources(2, 1, &splashSRV2);
		deviceContext->PSSetSamplers(1, 1, &m_splashSamplerPoint);

		int indexStart = meshSubsetIndexStart[i];
		int indexDrawAmount = meshSubsetIndexStart[i + 1] - meshSubsetIndexStart[i];
		if (!material[meshSubsetTexture[i]].transparent)
		deviceContext->DrawIndexed(indexDrawAmount, indexStart, 0);
	}
}

void RainClass::RenderObjT(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	//transparent

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//Set shaders
	deviceContext->VSSetShader(m_objvertexShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(m_objpixelShader, NULL, 0);

	//light
	ObjLightBufferType cbl;
	//cbl.light.pos = XMFLOAT3(lightPosition.x, lightPosition.y, lightPosition.z);
	cbl.light.pos = XMFLOAT3(256.0f, 256.0f, 256.0f);
	cbl.light.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	cbl.light.range = 1000.0f;
	cbl.light.cone = 20.0f;
	cbl.light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	cbl.light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	cbl.light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbl.light.padding = 0.0f;
	deviceContext->UpdateSubresource(m_objconstantBuffer2, 0, NULL, &cbl, 0, 0);

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	device->CreateBlendState(&blendDesc, &Transparency);

	//Set our Blend State
	deviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);

	for (int i = 0; i < meshSubsets; i++)
	{
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->IASetIndexBuffer(m_objindexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Set the vertex input layout
		deviceContext->IASetInputLayout(m_objlayout);

		//Set the vertex buffer to use
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_objvertexBuffer, &stride, &offset);

		//Set the constant buffer
		ObjMatrixBufferType cb;
		cb.world = worldMatrix;
		cb.view = viewMatrix;
		cb.projection = projectionMatrix;
		cb.difColor = material[meshSubsetTexture[i]].difColor;
		cb.hasTexture = material[meshSubsetTexture[i]].hasTexture;
		deviceContext->UpdateSubresource(m_objconstantBuffer1, 0, NULL, &cb, 0, 0);

		//VS
		deviceContext->VSSetConstantBuffers(0, 1, &m_objconstantBuffer1);

		//PS
		deviceContext->PSSetConstantBuffers(0, 1, &m_objconstantBuffer1);
		deviceContext->PSSetConstantBuffers(1, 1, &m_objconstantBuffer2);
		if (material[meshSubsetTexture[i]].hasTexture)
			deviceContext->PSSetShaderResources(0, 1, &objmeshSRV[material[meshSubsetTexture[i]].texArrayIndex]);
		deviceContext->PSSetSamplers(0, 1, &m_objSamplerPoint);

		int indexStart = meshSubsetIndexStart[i];
		int indexDrawAmount = meshSubsetIndexStart[i + 1] - meshSubsetIndexStart[i];
		if (material[meshSubsetTexture[i]].transparent)
			deviceContext->DrawIndexed(indexDrawAmount, indexStart, 0);
	}

}

void RainClass::RenderSLight(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
    worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);	

	//Set shaders
	deviceContext->VSSetShader(m_slightvertexShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(m_slightpixelShader, NULL, 0);

	//light
	ObjLightBufferType cbl;
	cbl.light.pos = XMFLOAT3(lightPosition.x, lightPosition.y, lightPosition.z);
	cbl.light.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
	cbl.light.range = 20.0f;
	cbl.light.cone = 20.0f;
	//cbl.light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	cbl.light.att = XMFLOAT3(0.4f, 0.02f, 0.000f);
	cbl.light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	cbl.light.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cbl.light.padding = 0.0f;
	deviceContext->UpdateSubresource(m_slightconstantBuffer2, 0, NULL, &cbl, 0, 0);

	//Nontransparent subsets
	for (int i = 0; i < slightmeshSubsets; i++)
	{
		//unbind
		ID3D11ShaderResourceView* nullSRV = nullptr;
		deviceContext->PSSetShaderResources(0, 1, &nullSRV);
		deviceContext->PSSetShaderResources(1, 1, &nullSRV);

		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->IASetIndexBuffer(m_slightindexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Set the vertex input layout
		deviceContext->IASetInputLayout(m_slightlayout);

		//Set the vertex buffer to use
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_slightvertexBuffer, &stride, &offset);

		//Set the constant buffer
		ObjMatrixBufferType cb;
		cb.world = worldMatrix;
		cb.view = viewMatrix;
		cb.projection = projectionMatrix;
		cb.difColor = slightmaterial[slightmeshSubsetTexture[i]].difColor;
		cb.hasTexture = slightmaterial[slightmeshSubsetTexture[i]].hasTexture;
		cb.padding = XMFLOAT2(0.0f, 0.0f);
		deviceContext->UpdateSubresource(m_slightconstantBuffer1, 0, NULL, &cb, 0, 0);

		//VS
		deviceContext->VSSetConstantBuffers(0, 1, &m_slightconstantBuffer1);

		//PS
		deviceContext->PSSetConstantBuffers(0, 1, &m_slightconstantBuffer1);
		deviceContext->PSSetConstantBuffers(1, 1, &m_slightconstantBuffer2);
		if (slightmaterial[slightmeshSubsetTexture[i]].hasTexture)
			deviceContext->PSSetShaderResources(0, 1, &slightmeshSRV[slightmaterial[slightmeshSubsetTexture[i]].texArrayIndex]);
		deviceContext->PSSetSamplers(0, 1, &m_slightSamplerPoint);

		int indexStart = slightmeshSubsetIndexStart[i];
		int indexDrawAmount = slightmeshSubsetIndexStart[i + 1] - slightmeshSubsetIndexStart[i];
		if (!slightmaterial[slightmeshSubsetTexture[i]].transparent)
			deviceContext->DrawIndexed(indexDrawAmount, indexStart, 0);
	}
}

void RainClass::RenderDLight(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0x00;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* depthStencilState;
	HRESULT result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* rasterState;
	device->CreateRasterizerState(&rasterDesc, &rasterState);
	deviceContext->RSSetState(rasterState);

	D3D11_BLEND_DESC blendStateDescription;
	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	ID3D11BlendState* BlendingState;
	result = device->CreateBlendState(&blendStateDescription, &BlendingState);
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	deviceContext->OMSetBlendState(BlendingState, blendFactor, 0xffffffff);

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//Set shaders
	deviceContext->VSSetShader(m_dlightvertexShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(m_dlightpixelShader, NULL, 0);

	//Nontransparent subsets
	for (int i = 0; i < dlightmeshSubsets; i++)
	{
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->IASetIndexBuffer(m_dlightindexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Set the vertex input layout
		deviceContext->IASetInputLayout(m_dlightlayout);

		//Set the vertex buffer to use
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_dlightvertexBuffer, &stride, &offset);

		//Set the constant buffer
		ObjMatrixBufferType cb;
		cb.world = worldMatrix;
		cb.view = viewMatrix;
		cb.projection = projectionMatrix;
		//cb.difColor = dlightmaterial[dlightmeshSubsetTexture[i]].difColor;
		//cb.hasTexture = dlightmaterial[dlightmeshSubsetTexture[i]].hasTexture;
		cb.difColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		cb.hasTexture = 0;
		cb.padding = XMFLOAT2(0.0f, 0.0f);
		deviceContext->UpdateSubresource(m_dlightconstantBuffer, 0, NULL, &cb, 0, 0);

		//VS
		deviceContext->VSSetConstantBuffers(0, 1, &m_dlightconstantBuffer);

		//PS
		int indexStart = dlightmeshSubsetIndexStart[i];
		int indexDrawAmount = dlightmeshSubsetIndexStart[i + 1] - dlightmeshSubsetIndexStart[i];
		//if (!dlightmaterial[dlightmeshSubsetTexture[i]].transparent)
			deviceContext->DrawIndexed(indexDrawAmount, indexStart, 0);
	}
}

//Load a texture array and associated view from a series of textures on disk
bool RainClass::LoadTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* sTexturePrefix, int iNumTextures, ID3D11Texture2D** texture, ID3D11ShaderResourceView** resourceView)
{
	HRESULT result = S_OK;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	WCHAR szTextureName[MAX_PATH];

	for (int i = 0; i < iNumTextures; i++)
	{
		wsprintf(szTextureName, L"%S%.4d.dds", sTexturePrefix, i);

		ID3D11Texture2D* Res = NULL;
	
		result = CreateDDSTextureFromFileEx(device, 
			                                szTextureName,
			                                NULL,
			                                D3D11_USAGE_IMMUTABLE,
			                                D3D11_BIND_SHADER_RESOURCE,
			                                NULL,
			                                NULL,
			                                false,
			                                reinterpret_cast<ID3D11Resource**>(&Res), 
			                                nullptr);
		if (FAILED(result))
			return false;

		if (Res)
		{
			Res->GetDesc(&desc);
			D3D11_TEXTURE2D_DESC texDesc;
			texDesc.Width = desc.Width;
			texDesc.Height = desc.Height;
			texDesc.MipLevels = desc.MipLevels;
			texDesc.ArraySize = iNumTextures;
			texDesc.Format = desc.Format;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags = NULL;
			texDesc.MiscFlags = NULL;

			if (DXGI_FORMAT_R8_UNORM != desc.Format)
				return false;

			if (!(*texture))
			{
				result = device->CreateTexture2D(&texDesc, NULL, texture);
				if (FAILED(result))
					return false;
			}

			for (UINT iMip = 0; iMip < desc.MipLevels; iMip++)
			{
				deviceContext->CopySubresourceRegion(*texture,
					                                 D3D11CalcSubresource(iMip,i,texDesc.MipLevels),
					                                 NULL,
					                                 NULL,
					                                 NULL,
					                                 Res,
					                                 iMip,
					                                 nullptr);
			}

			if (Res != NULL)
				Res->Release();
		}
		else
			return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = iNumTextures;
	result = device->CreateShaderResourceView(*texture, &SRVDesc, resourceView);
	if (FAILED(result))
		return false;

	return true;
}

bool RainClass::loadLUTS(char* fileName, int xRes, int yRes, ID3D11Device* device)
{
	HRESULT result;

	ifstream infile(fileName, ios::in);
	if (infile.is_open())
	{
		float* data = new float[xRes*yRes];
		int index = 0;
		char tempc;
		for (int j = 0; j < yRes-1; j++)
		{
			for (int i = 0; i < xRes-1; i++)
				infile >> data[index++] >> tempc;
			infile >> data[index++];
		}

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.SysMemPitch = sizeof(float)*xRes;
		InitData.pSysMem = data;

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = xRes;
		texDesc.Height = yRes;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		result = device->CreateTexture2D(&texDesc, &InitData, &m_skytexture);
		if (FAILED(result))
			return false;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = texDesc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		result = device->CreateShaderResourceView(m_skytexture, &SRVDesc, &m_skySRV);
		if (FAILED(result))
			return false;

		delete[] data;

		return true;
	}
	else
		return false;
}

HRESULT RainClass::CreateRainFactorsResource(ID3D11Device* device)
{
	D3D11_TEXTURE1D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = 370;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_SINT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.SysMemPitch = 16 * sizeof(int);
	initData.SysMemSlicePitch = 0;

	initData.pSysMem = m_RainFactors;
	ID3D11Texture1D* texture = NULL;
	HRESULT result = device->CreateTexture1D(&desc, &initData, &texture);
	if (FAILED(result))
		return result;

	result = device->CreateShaderResourceView(texture, NULL, &m_rainFactorSRV);
	return result;
}

bool RainClass::LoadObjModel(ID3D11Device* device,
	                         std::wstring filename,
	                         ID3D11Buffer** vertBuff,
	                         ID3D11Buffer** indexBuff,
	                         vector<int>& subsetIndexStart,
	                         vector<int>& subsetMaterialArray, //index value of material for each subset
	                         vector<SurfaceMaterial>& material,//vector of material structures
	                         int& subsetCount,                 //number of subsets in mesh
	                         vector<ID3D11ShaderResourceView*>& meshSRV,
	                         bool isRHCoordSys,                //true if model was created in right hand coord system
	                         bool computeNormals)              //true to compute the normals, false to use the files normals
{
	HRESULT result;
	wifstream fileIn(filename.c_str());
	wstring meshMatLib; //String to hold our obj material library filenanme

						//Arrays to store our model's information
	vector<DWORD> indices;
	vector<XMFLOAT3> vertPos;
	vector<XMFLOAT3> vertNorm;
	vector<XMFLOAT2> vertTexCoord;
	vector<wstring> meshMaterials;

	//Vertex definition indices
	vector<int> vertPosIndex;
	vector<int> vertNormIndex;
	vector<int> vertTCIndex;

	bool hasTexCoord = false;
	bool hasNorm = false;
	bool hasMat = false;

	wstring meshMaterialsTemp;
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTCIndexTemp;

	wchar_t checkChar;     //The variable we will use to store one char from file at a time
	wstring face;          //Hold the string containing our face vertices
	int vIndex = 0;        //Keep track of our vertex index count
	int triangleCount = 0; //Total Triangles
	int totalVerts = 0;
	int meshTriangles = 0;

	if (fileIn)
	{
		while (fileIn)
		{
			checkChar = fileIn.get();

			switch (checkChar)
			{
			case '#':  //Comment
				checkChar = fileIn.get();
				while (checkChar != '\n')
					checkChar = fileIn.get();
				break;
			case 'v':  //Get Vertex Descriptions
				checkChar = fileIn.get();
				if (checkChar == ' ') //v - vert position
				{
					float vz, vy, vx;
					fileIn >> vx >> vy >> vz;

					if (isRHCoordSys)
						vertPos.push_back(XMFLOAT3(vx, vy, vz*-1.0f));
					else
						vertPos.push_back(XMFLOAT3(vx, vy, vz));
				}

				if (checkChar == 't') //vt - vert tex coords
				{
					float vtcu, vtcv;
					fileIn >> vtcu >> vtcv;

					if (isRHCoordSys)
						vertTexCoord.push_back(XMFLOAT2(vtcu, 1.0f - vtcv));
					else
						vertTexCoord.push_back(XMFLOAT2(vtcu, vtcv));

					hasTexCoord = true;
				}

				if (checkChar == 'n')
				{
					float vnx, vny, vnz;
					fileIn >> vnx >> vny >> vnz;

					if (isRHCoordSys)
						vertNorm.push_back(XMFLOAT3(vnx, vny, vnz*-1.0f));
					else
						vertNorm.push_back(XMFLOAT3(vnx, vny, vnz));

					hasNorm = true;
				}
				break;
			case 'g':  //g - defines a group, New Group(Subset)
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					subsetIndexStart.push_back(vIndex); //Start index for this subset
					subsetCount++;
				}
				break;
			//case 'o':  //o - defines a group of groups
			//	checkChar = fileIn.get();
			//	if (checkChar == ' ')
			//	{
			//		subsetIndexStart.push_back(vIndex); //Start index for this subset
			//		subsetCount++;
			//	}
				break;
			case 'f':  //f - defines the faces
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					face = L"";
					wstring VertDef;
					triangleCount = 0;

					checkChar = fileIn.get();
					while (checkChar != '\n')
					{
						face += checkChar;
						checkChar = fileIn.get();
						if (checkChar == ' ')
							triangleCount++;
					}

					if (face[face.length() - 1] == ' ')
						triangleCount--;

					triangleCount -= 1;

					wstringstream ss(face);

					if (face.length() > 0)
					{
						int firstVIndex, lastVIndex;

						for (int i = 0; i < 3; i++)
						{
							ss >> VertDef; //Get vertex definition (vPos/vTexCoord/vNorm)

							std::wstring vertPart;
							int whichPart = 0;

							for (int j = 0; j < VertDef.length(); ++j)
							{
								if (VertDef[j] != '/')
									vertPart += VertDef[j];

								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									wistringstream wstringToInt(vertPart);

									if (whichPart == 0) //vPos
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1; //subtract one since c++ arrays start with 0, and obj start with 1

										if (j == VertDef.length() - 1)
										{
											vertNormIndexTemp = 0;
											vertTCIndexTemp = 0;
										}
									}
									else if (whichPart == 1) //vTexCoord
									{
										if (vertPart != L"")
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;
										}
										else
											vertTCIndexTemp = 0;

										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;
									}
									else if (whichPart == 2) //vNorm
									{
										wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;
									}

									vertPart = L"";
									whichPart++;
								}
							}

							if (subsetCount == 0)
							{
								subsetIndexStart.push_back(vIndex);
								subsetCount++;
							}

							bool vertAlreadyExists = false;
							if (totalVerts >= 3)
							{
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											indices.push_back(iCheck);
											vertAlreadyExists = true;
										}
									}
								}
							}

							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++; //We created a mew vertex
								indices.push_back(totalVerts - 1); //Set index for this vertex
							}

							//If this is the very first vertex in the face
							//we need to make sure the rest of the triangle use this vertex
							if (i == 0)
								firstVIndex = indices[vIndex];

							//If this was the last vertex in the first triangle
							//the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
							if (i == 2)
								lastVIndex = indices[vIndex];

							vIndex++;
						}
						meshTriangles++;

						for (int l = 0; l < triangleCount - 1; ++l)
						{
							indices.push_back(firstVIndex);
							vIndex++;

							indices.push_back(lastVIndex);
							vIndex++;

							ss >> VertDef;

							wstring vertPart;
							int whichPart = 0;

							for (int j = 0; j < VertDef.length(); ++j)
							{
								if (VertDef[j] != '/')
									vertPart += VertDef[j];
								if (VertDef[j] == '/' || j == VertDef.length() - 1)
								{
									wistringstream wstringToInt(vertPart);

									if (whichPart == 0)
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;

										if (j == VertDef.length() - 1)
										{
											vertTCIndexTemp = 0;
											vertNormIndexTemp = 0;
										}
									}
									else if (whichPart == 1)
									{
										if (vertPart != L"")
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;
										}
										else
											vertTCIndexTemp = 0;
										if (j == VertDef.length() - 1)
											vertNormIndexTemp = 0;
									}
									else if (whichPart == 2)
									{
										wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;
									}

									vertPart = L"";
									whichPart++;
								}
							}

							bool vertAlreadyExists = false;
							if (totalVerts >= 3)
							{
								for (int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									if (vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if (vertTCIndexTemp == vertTCIndex[iCheck])
										{
											indices.push_back(iCheck);
											vertAlreadyExists = true;
										}
									}
								}
							}

							if (!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;
								indices.push_back(totalVerts - 1);
							}

							//Set the second vertex for the next triangles to the last vertex we got
							lastVIndex = indices[vIndex];

							meshTriangles++;
							vIndex++;
						}
					}
				}
				break;

			case 'm':  //mtllib - material library filename
				checkChar = fileIn.get();
				if (checkChar == 't')
				{
					checkChar = fileIn.get();
					if (checkChar == 'l')
					{
						checkChar = fileIn.get();
						if (checkChar == 'l')
						{
							checkChar = fileIn.get();
							if (checkChar == 'i')
							{
								checkChar = fileIn.get();
								if (checkChar == 'b')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										fileIn >> meshMatLib;
										hasMat = true;
									}
								}
							}
						}
					}
				}
				break;

			case 'u':  //usemtl - which material to use
				checkChar = fileIn.get();
				if (checkChar == 's')
				{
					checkChar = fileIn.get();
					if (checkChar == 'e')
					{
						checkChar = fileIn.get();
						if (checkChar == 'm')
						{
							checkChar = fileIn.get();
							if (checkChar == 't')
							{
								checkChar = fileIn.get();
								if (checkChar == 'l')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										meshMaterialsTemp = L"";
										fileIn >> meshMaterialsTemp;
										meshMaterials.push_back(meshMaterialsTemp);
									}
								}
							}
						}
					}
				}
				break;

			default:
				break;
			}
		}
	}
	else
	{
		wstring message = L"Could not open: ";
		message += filename;

		MessageBox(0, message.c_str(), L"Error", MB_OK);
		return false;
	}

	subsetIndexStart.push_back(vIndex);

	//sometimes "g" is defined at the very top of the file, then again before the first group of faces
	//This makes sure the first subset does not contain "0" indices
	if (subsetIndexStart.size()>1 && subsetIndexStart[1] == 0)
	{
		subsetIndexStart.erase(subsetIndexStart.begin() + 1);
		//meshSubsets--;
		subsetCount--;
	}

	if (!hasNorm)
		vertNorm.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
	if (!hasTexCoord)
		vertTexCoord.push_back(XMFLOAT2(0.0f, 0.0f));

	fileIn.close();
	if (hasMat)
	    fileIn.open(meshMatLib.c_str());

	wstring lastStringRead;
	int matCount = material.size();

	bool kdset = false;

	if (fileIn && hasMat)
	{
		while (fileIn)
		{
			checkChar = fileIn.get();

			switch (checkChar)
			{
			case '#': //comment
				checkChar = fileIn.get();
				while (checkChar != '\n')
					checkChar = fileIn.get();
				break;

			case 'K': //diffuse color
				checkChar = fileIn.get();
				if (checkChar == 'd') //diffuse
				{
					checkChar = fileIn.get(); //remove space

					fileIn >> material[matCount - 1].difColor.x;
					fileIn >> material[matCount - 1].difColor.y;
					fileIn >> material[matCount - 1].difColor.z;

					kdset = true;
				}

				if (checkChar == 'a') //ambient
				{
					checkChar = fileIn.get();
					//store it in diffuse if there isn't a diffuse already
					if (!kdset)
					{
						fileIn >> material[matCount - 1].difColor.x;
						fileIn >> material[matCount - 1].difColor.y;
						fileIn >> material[matCount - 1].difColor.z;
					}
				}
				break;

			case 'T': //transparency
				checkChar = fileIn.get();
				if (checkChar == 'r')
				{
					checkChar = fileIn.get();
					float Transparency;
					fileIn >> Transparency;

					material[matCount - 1].difColor.w = Transparency;

					if (Transparency > 0.0f)
						material[matCount - 1].transparent = true;
				}
				break;

			case 'd': //some obj file specifies d for transparency
				checkChar = fileIn.get();
				if (checkChar == ' ')
				{
					float Transparency;
					fileIn >> Transparency;

					//'d' - 0 being most transparent, and 1 being opaque, opposite of Tr
					Transparency = 1.0f - Transparency;

					material[matCount - 1].difColor.w = Transparency;

					if (Transparency > 0.0f)
						material[matCount - 1].transparent = true;
				}
				break;

			case 'm': //diffuse map (texture)
				checkChar = fileIn.get();
				if (checkChar == 'a')
				{
					checkChar = fileIn.get();
					if (checkChar == 'p')
					{
						checkChar = fileIn.get();
						if (checkChar == '_')
						{
							//map_Kd - Diffuse map
							checkChar = fileIn.get();
							if (checkChar == 'K')
							{
								checkChar = fileIn.get();
								if (checkChar == 'd')
								{
									wstring fileNamePath;

									fileIn.get(); //remove space

												  //Get the file path
												  //We read the path name char by char
												  //pathnames can sometimes contain space, so we will read until file extension
									bool texFilePathEnd = false;
									while (!texFilePathEnd)
									{
										checkChar = fileIn.get();
										fileNamePath += checkChar;
										if (checkChar == '.')
										{
											for (int i = 0; i < 3; ++i)
												fileNamePath += fileIn.get();

											texFilePathEnd = true;
										}
									}

									//Check if this texture has already been loaded
									bool alreadyLoaded = false;
									for (int i = 0; i < textureNameArray.size(); ++i)
									{
										if (fileNamePath == textureNameArray[i])
										{
											alreadyLoaded = true;
											material[matCount - 1].texArrayIndex = i;
											material[matCount - 1].hasTexture = true;
										}
									}

									//if the texture is not already loaded, load it now
									if (!alreadyLoaded)
									{
										ID3D11ShaderResourceView* tempMeshSRV;
										ID3D11Resource* tempTexture;
										result = CreateWICTextureFromFile(device,
											                              fileNamePath.c_str(),
											                              &tempTexture,
											                              &tempMeshSRV,
											                              0);
										if (SUCCEEDED(result))
										{
											textureNameArray.push_back(fileNamePath.c_str());
											material[matCount - 1].texArrayIndex = meshSRV.size();
											meshSRV.push_back(tempMeshSRV);
											material[matCount - 1].hasTexture = 1;
										}
									}
								}
							}
							//map_d - alpha map
							else if (checkChar == 'd')
							{
								//Alpha maps are usually the same as the diffuse map
								//So we will assume that for now by only enabling
								//transparency forthis material, as we will already
								//be using the alpha channel in the diffuse map
								material[matCount - 1].transparent = true;
							}
						}
					}
				}
				break;

			case 'n':  //newmtl - Declare new material
				checkChar = fileIn.get();
				if (checkChar == 'e')
				{
					checkChar = fileIn.get();
					if (checkChar == 'w')
					{
						checkChar = fileIn.get();
						if (checkChar == 'm')
						{
							checkChar = fileIn.get();
							if (checkChar == 't')
							{
								checkChar = fileIn.get();
								if (checkChar == 'l')
								{
									checkChar = fileIn.get();
									if (checkChar == ' ')
									{
										SurfaceMaterial tempMat;
										material.push_back(tempMat);
										fileIn >> material[matCount].matName;
										material[matCount].transparent = false;
										WCHAR compare[5] = L"None";
										if(material[matCount].matName == compare)
											material[matCount].specular = true;
										else
											material[matCount].specular = false;
										material[matCount].hasTexture = 0;
										material[matCount].texArrayIndex = 0;
										matCount++;
										kdset = false;
									}
								}
							}
						}
					}
				}
				break;

			default:
				break;
			}
		}
	}
	else if (hasMat == false)
	{

	}
	else
	{
		wstring message = L"Could not open: ";
		message += meshMatLib;

		MessageBox(0, message.c_str(), L"Error", MB_OK);
		return false;
	}

	//Set the subsets material to the index value
	//of the its material in our material array
	//for (int i = 0; i < meshSubsets; ++i)
	for (int i = 0; i < subsetCount; ++i)
	{
		bool hasMat = false;
		for (int j = 0; j < material.size(); ++j)
		{
			if (meshMaterials[i] == material[j].matName)
			{
				subsetMaterialArray.push_back(j);
				hasMat = true;
			}
		}
		if (!hasMat)
			subsetMaterialArray.push_back(0); //use first material in array
	}

	vector<Vertex> vertices;
	Vertex tempVert;

	for (int j = 0; j < totalVerts; ++j)
	{
		tempVert.pos = vertPos[vertPosIndex[j]];
		tempVert.normal = vertNorm[vertNormIndex[j]];
		tempVert.texCoord = vertTexCoord[vertTCIndex[j]];

		vertices.push_back(tempVert);
	}

	//Compute Normals
	if (computeNormals)
	{
		vector<XMFLOAT3> tempNormal;

		XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

		float vecX, vecY, vecZ;

		XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//compute face normals
		for (int i = 0; i < meshTriangles; ++i)
		{
			//get the vector of edge 0,2
			vecX = vertices[indices[(i * 3)]].pos.x - vertices[indices[(i * 3) + 2]].pos.x;
			vecY = vertices[indices[(i * 3)]].pos.y - vertices[indices[(i * 3) + 2]].pos.y;
			vecZ = vertices[indices[(i * 3)]].pos.z - vertices[indices[(i * 3) + 2]].pos.z;
			edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);

			//get the vector of edge 2,1
			vecX = vertices[indices[(i * 3) + 2]].pos.x - vertices[indices[(i * 3) + 1]].pos.x;
			vecY = vertices[indices[(i * 3) + 2]].pos.y - vertices[indices[(i * 3) + 1]].pos.y;
			vecZ = vertices[indices[(i * 3) + 2]].pos.z - vertices[indices[(i * 3) + 1]].pos.z;
			edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);

			//cross multiply to get the un-normalized face normal
			XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
			tempNormal.push_back(unnormalized);
		}

		//compute vertex normals
		XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		int facesUsing = 0;
		float tX;
		float tY;
		float tZ;

		//go through each vertex
		for (int i = 0; i < totalVerts; ++i)
		{
			//check which triangles use this vertex
			for (int j = 0; j < meshTriangles; ++j)
			{
				if (indices[j * 3] == i || indices[(j * 3) + 1] == i || indices[(j * 3) + 2] == i)
				{
					tX = XMVectorGetX(normalSum) + tempNormal[j].x;
					tY = XMVectorGetY(normalSum) + tempNormal[j].y;
					tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

					normalSum = XMVectorSet(tX, tY, tZ, 0.0f);
					facesUsing++;
				}
			}

			normalSum = normalSum / facesUsing;
			normalSum = XMVector3Normalize(normalSum);

			vertices[i].normal.x = XMVectorGetX(normalSum);
			vertices[i].normal.y = XMVectorGetY(normalSum);
			vertices[i].normal.z = XMVectorGetZ(normalSum);

			normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			facesUsing = 0;
		}
	}

	//Create index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD)*meshTriangles * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	device->CreateBuffer(&indexBufferDesc, &iinitData, indexBuff);

	//Create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex)*totalVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	result = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertBuff);

	return true;
}


bool RainClass::Load3DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* filename, ID3D11Texture3D** texture, ID3D11ShaderResourceView** resourceView)
{
	HRESULT result;

	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE3D_DESC));

	ID3D11Texture3D* Res = NULL;

	result = CreateDDSTextureFromFileEx(device,
		                                filename,
		                                NULL,
		                                D3D11_USAGE_IMMUTABLE,
		                                D3D11_BIND_SHADER_RESOURCE,
		                                NULL,
		                                NULL,
		                                false,
		                                reinterpret_cast<ID3D11Resource**>(&Res),
		                                nullptr);
	if (FAILED(result))
		return false;

	if (Res)
	{
		Res->GetDesc(&desc);
		D3D11_TEXTURE3D_DESC texDesc;
		texDesc.Width          = desc.Width;
		texDesc.Height         = desc.Height;
		texDesc.Depth          = desc.Depth;
		texDesc.MipLevels      = desc.MipLevels;
		texDesc.Format         = desc.Format;
		texDesc.Usage          = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = NULL;
		texDesc.MiscFlags      = desc.MiscFlags;

		//if (DXGI_FORMAT_R8_UNORM != desc.Format)
		//	return false;

		if (!(*texture))
		{
			result = device->CreateTexture3D(&texDesc, NULL, texture);
			if (FAILED(result))
				return false;
		}

		for (UINT iMip = 0; iMip < desc.MipLevels; iMip++)
		{
			deviceContext->CopySubresourceRegion(*texture,
				                                 D3D11CalcSubresource(iMip, 0, texDesc.MipLevels),
				                                 NULL,
				                                 NULL,
				                                 NULL,
				                                 Res,
				                                 iMip,
				                                 nullptr);
		}

		if (Res != NULL)
			Res->Release();
	}
	else
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	SRVDesc.Texture3D.MipLevels = desc.MipLevels;
	result = device->CreateShaderResourceView(*texture, &SRVDesc, resourceView);
	if (FAILED(result))
		return false;

	return true;
}