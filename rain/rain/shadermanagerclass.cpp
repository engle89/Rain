#include "shadermanagerclass.h"

ShaderManagerClass::ShaderManagerClass()
{
	m_rain = 0;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{

}


ShaderManagerClass::~ShaderManagerClass()
{

}

bool ShaderManagerClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	bool result;

	m_rain = new RainClass();
	result = m_rain->Initialize(device, deviceContext, hwnd);
	if (!result)
		return false;

	return true;
}


void ShaderManagerClass::Shutdown()
{
	if (m_rain)
	{
		m_rain->Shutdown();
		delete m_rain;
		m_rain = 0;
	}

	return;
}


bool ShaderManagerClass::RenderRainShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX worldMatrix2, XMMATRIX worldMatrix3, XMMATRIX worldMatrix4, XMMATRIX worldMatrix5, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 lightPosition)
{

	bool v1 = m_rain->Render(device, deviceContext, worldMatrix, worldMatrix2, worldMatrix3, worldMatrix4, worldMatrix5, viewMatrix, projectionMatrix, lightPosition);
	return v1;
}
