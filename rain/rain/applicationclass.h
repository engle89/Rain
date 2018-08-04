#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

//globals
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 0.01f;

#include "inputclass.h"
#include "d3dclass.h"
#include "shadermanagerclass.h"
#include "fpsclass.h"
#include "zoneclass.h"
#include "timerclass.h"

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	InputClass * m_Input;
	D3DClass* m_Direct3D;
	ShaderManagerClass* m_ShaderManager;
	FpsClass* m_Fps;
	ZoneClass* m_Zone;
	TimerClass* m_Timer;
};

#endif
