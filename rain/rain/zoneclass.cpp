#include "zoneclass.h"

ZoneClass::ZoneClass()
{
	m_Camera = 0;
	m_Position = 0;
}

ZoneClass::ZoneClass(const ZoneClass& other)
{

}

ZoneClass::~ZoneClass()
{

}

bool ZoneClass::Initialize(D3DClass* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
{
	m_hwnd = hwnd;

	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	//m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->SetPosition(0.0f, 0.0f, 0.0f);
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();


	m_Position = new PositionClass;
	if (!m_Position)
		return false;

	//m_Position->SetPosition(128.0f, 10.0f, -10.0f);
	m_Position->SetPosition(0.0f, 0.0f, 0.0f);
	m_Position->SetRotation(0.0f, 0.0f, 0.0f);

	m_displayUI = true;

	m_wireFrame = false;

	return true;
}

void ZoneClass::Shutdown()
{
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	return;
}

bool ZoneClass::Frame(D3DClass* Direct3D, InputClass* Input, ShaderManagerClass* ShaderManager, float frameTime, int fps)
{
	bool result;
	float posX = 0.0f, posY = 0.0f, posZ = 0.0f, rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

	HandleMovementInput(Input, frameTime);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	//result = m_UserInterface->Frame(Direct3D->GetDeviceContext(), fps, posX, posY, posZ, rotX, rotY, rotZ);
	//if (!result)
	//return false;

	result = Render(Direct3D, ShaderManager);
	if (!result)
		return false;

	return true;
}

void ZoneClass::HandleMovementInput(InputClass* Input, float frameTime)
{
	bool keyDown;
	float posX = 0.0f, posY = 0.0f, posZ = 0.0f, rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

	m_Position->setFrameTime(frameTime);

	keyDown = Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = Input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = Input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = Input->IsAPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = Input->IsZPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);
	m_Camera->SetYaw(Input->GetYaw());
	m_Camera->SetPitch(Input->GetPitch());

	if (Input->IsF1Toggled())
		m_displayUI = !m_displayUI;

	if (Input->IsF2Toggled())
		m_wireFrame = !m_wireFrame;

	return;
}

bool ZoneClass::Render(D3DClass* Direct3D, ShaderManagerClass* ShaderManager)
{
	XMMATRIX worldMatrix, worldMatrix2, worldMatrix3, worldMatrix4, worldMatrix5, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	m_Camera->Render();

	Direct3D->GetWorldMatrix(worldMatrix3);

	worldMatrix = XMMatrixSet(0, 0, 1, 0,
	                          0, 1, 0, 0,
		                      -1, 0, 0, 0,
		                     -4, -1, 10, 1);
	
	//worldMatrix2 = XMMatrixSet(0, 0, 0.1, 0,
	//	                       0, 0.2, 0, 0,
	//	                       -0.1, 0, 0, 0,
	//	                      2, -3.5, 5.4, 1);
	XMFLOAT3 pos = m_Camera->GetPosition();
	worldMatrix2 = XMMatrixSet(5, 0, 0, 0,
		                       0, 5, 0, 0,
		                       0, 0, 5, 0,
		                       pos.x, pos.y, pos.z, 1);

	//worldMatrix3 = XMMatrixSet(1, 0, 0, 0,
	//	                       0, 1, 0, 0,
	//                           0, 0, 1, 0,
	//                           pos.x, pos.y, pos.z, 1);

	worldMatrix4 = XMMatrixSet(0.02, 0, 0, 0,
		                       0, 0.02, 0, 0,
		                       0, 0, 0.02, 0,
		                       0, 0,  0, 1);

	worldMatrix5 = XMMatrixSet(0, 0, -0.4, 0,
		                       0, 0.4, 0, 0,
		                       -0.4, 0, 0, 0,
		                       2.0, -3.4, 5.4, 1);

	//XMFLOAT3 ArrowPos = XMFLOAT3(-5.51748, -4.31354, -4.00869);
	XMFLOAT3 ArrowPos = XMFLOAT3(5.51748, 7.31354, 4.00869);
	XMFLOAT3 LightPos = XMFLOAT3(-551.748, -728.354, -400.869);
	XMFLOAT3 up       = XMFLOAT3(0.0, 1.0, 0.0);
	XMFLOAT3 origin   = XMFLOAT3(0.0, 0.0, 0.0);
	XMVECTOR upVector, positionVector, originVector;
	upVector       = XMLoadFloat3(&up);
	positionVector = XMLoadFloat3(&ArrowPos);
	originVector   = XMLoadFloat3(&origin);
	XMMATRIX LookAt = XMMatrixLookAtLH(positionVector, originVector, upVector);
	XMMATRIX LookAtInv = XMMatrixInverse(NULL, LookAt);
	worldMatrix4 = XMMatrixMultiply(worldMatrix4, LookAtInv);
	worldMatrix4 = XMMatrixMultiply(worldMatrix4, worldMatrix3);

	m_Camera->GetViewMatrix(viewMatrix);

	Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	Direct3D->GetOrthoMatrix(orthoMatrix);

	//Clear the buffer to begin the scene
	//Direct3D->BeginScene(0.3f, 0.4f, 0.7f, 1.0f);
	Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	Direct3D->DisableAlphaBlending();

	if (m_wireFrame)
		Direct3D->EnableWireframe();

	XMFLOAT4 lightPosition = XMFLOAT4(1.0f, 0.70f, 5.4f, 1.0f);

	bool v = true;
	
	result = ShaderManager->RenderRainShader(Direct3D->GetDevice(),Direct3D->GetDeviceContext(), worldMatrix, worldMatrix2, worldMatrix3, worldMatrix4, worldMatrix5, viewMatrix, projectionMatrix, lightPosition);
	if (!result)
		return false;

	if (m_wireFrame)
		Direct3D->DisableWireframe();

	Direct3D->EndScene();

	return true;
}