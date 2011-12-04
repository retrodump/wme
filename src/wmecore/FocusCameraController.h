// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFocusCameraController_H__
#define __WmeFocusCameraController_H__


#include "CameraController.h"


namespace Wme
{
	class FocusCameraController : public CameraController
	{
	public:
		FocusCameraController();
		virtual ~FocusCameraController();

		bool HandleMouseEvent(MouseEvent& event);
		void Update();

	private:
		bool m_IsDragging;

		float m_PrevMouseX;
		float m_PrevMouseY;
		void CenterMouse();

		void Orbit(Ogre::Camera* camera, const Ogre::Vector3& point, float delta, bool horizontal);
		void Zoom(Ogre::Camera* camera, const Ogre::Vector3& point, float delta);
		void Pan(Ogre::Camera* camera, float deltaX, float deltaY);
		void ZoomWheel(bool zoomIn);
		void ZoomOrthographic(float delta);

		bool GetFocusObject();
		Ogre::Entity* m_FocusObject;
		Ogre::Vector3 m_FocusOffset;
	};
}

#endif // __WmeFocusCameraController_H__
