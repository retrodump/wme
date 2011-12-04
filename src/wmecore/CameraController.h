// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCameraController_H__
#define __WmeCameraController_H__


#include "Object.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"


namespace Wme
{
	class Viewport;
	class Camera;
	class CameraEntity;

	class CameraController : public Object
	{
	public:
		CameraController();
		virtual ~CameraController();

		virtual void Create(Viewport* viewport);

		Viewport* GetViewport() const { return m_Viewport; }
		
		Camera* GetEditingCamera() const { return m_EditingCamera; }

		void CloneCamera(Camera* camera);
		void SaveCameraTo(CameraEntity* targetCamera);

		virtual bool HandleMouseEvent(MouseEvent& event) { return false; };
		virtual bool HandleKeyboardEvent(KeyboardEvent& event) { return false; };
		virtual void Update() {};

	protected:
		Viewport* m_Viewport;
		Camera* m_EditingCamera;
	};
}

#endif // __WmeCameraController_H__
