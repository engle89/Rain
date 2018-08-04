#ifndef _ZONECLASS_H_
#define _ZONECLASS_H_

#include "d3dclass.h"
#include "inputclass.h"
#include "shadermanagerclass.h"
#include "cameraclass.h"
#include "positionclass.h"

class ZoneClass
{
public:
	ZoneClass();
	ZoneClass(const ZoneClass&);
	~ZoneClass();

	bool Initialize(D3DClass*, HWND, int, int, float);
	void Shutdown();
	bool Frame(D3DClass*, InputClass*, ShaderManagerClass*, float, int);

private:
	void HandleMovementInput(InputClass*, float);
	bool Render(D3DClass*, ShaderManagerClass*);

private:
	CameraClass * m_Camera;
	PositionClass* m_Position;
	bool m_displayUI;
	bool m_wireFrame;
	HWND m_hwnd;
};

#endif
