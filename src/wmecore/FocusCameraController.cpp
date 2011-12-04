// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "FocusCameraController.h"
#include "Camera.h"
#include "Viewport.h"
#include "Stage.h"
#include "Game.h"
#include "InputManager.h"
#include "ContentManager.h"
#include "StringUtil.h"
#include "MousePickParams.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
FocusCameraController::FocusCameraController() : CameraController()
{
	m_PrevMouseX = m_PrevMouseY = 0.0f;
	m_IsDragging = false;

	m_FocusObject = NULL;
	m_FocusOffset = Ogre::Vector3::ZERO;
}

//////////////////////////////////////////////////////////////////////////
FocusCameraController::~FocusCameraController()
{
}

//////////////////////////////////////////////////////////////////////////
bool FocusCameraController::HandleMouseEvent(MouseEvent& event)
{
	if (event.GetButton() == MouseEvent::BUTTON_LEFT && event.GetType() == MouseEvent::EVENT_PRESS)
	{
		if (!m_IsDragging && Game::GetInstance()->GetContentMgr()->IsAltDown() && GetFocusObject())
		{
			Game::GetInstance()->GetContentMgr()->CaptureMouse(m_Viewport);

			m_EditingCamera->GetOgreCamera()->setFixedYawAxis(true);
			m_EditingCamera->GetOgreCamera()->setAutoTracking(true, m_FocusObject->getParentSceneNode(), m_FocusOffset);
			m_EditingCamera->GetOgreCamera()->_autoTrack();


			CenterMouse();
			m_IsDragging = true;		
			return true;
		}
	}
	else if (event.GetButton() == MouseEvent::BUTTON_LEFT && event.GetType() == MouseEvent::EVENT_RELEASE)
	{
		if (m_IsDragging)
		{
			Game::GetInstance()->GetContentMgr()->ReleaseMouse(m_Viewport);
			m_IsDragging = false;

			m_EditingCamera->GetOgreCamera()->setAutoTracking(false);

			return true;
		}
	}
	else if (event.GetType() == MouseEvent::EVENT_WHEEL_UP || event.GetType() == MouseEvent::EVENT_WHEEL_DOWN)
	{
		ZoomWheel(event.GetType() == MouseEvent::EVENT_WHEEL_UP);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void FocusCameraController::Update()
{
	if (!m_IsDragging) return;


	float x, y;
	m_Viewport->GetRelativeMousePos(x, y);

	float deltaX = x - m_PrevMouseX;
	float deltaY = y - m_PrevMouseY;

	if (deltaX == 0.0f && deltaY == 0.0f) return;


	if (Game::GetInstance()->GetContentMgr()->IsControlDown() && Game::GetInstance()->GetContentMgr()->IsShiftDown())
	{
		// panning
		Pan(m_EditingCamera->GetOgreCamera(), deltaX, deltaY);
	}
	else
	{
		// orbit horizontal
		Orbit(m_EditingCamera->GetOgreCamera(), m_FocusObject->getParentSceneNode()->_getDerivedPosition() + m_FocusOffset, deltaX, true);

		// orbit vertical or zoom
		if (Game::GetInstance()->GetContentMgr()->IsControlDown())
		{
			Orbit(m_EditingCamera->GetOgreCamera(), m_FocusObject->getParentSceneNode()->_getDerivedPosition() + m_FocusOffset, deltaY, false);
		}
		else
		{
			Zoom(m_EditingCamera->GetOgreCamera(), m_FocusObject->getParentSceneNode()->_getDerivedPosition() + m_FocusOffset, deltaY);
		}
	}

	CenterMouse();
}


//////////////////////////////////////////////////////////////////////////
void FocusCameraController::Orbit(Ogre::Camera* camera, const Ogre::Vector3& point, float delta, bool horizontal)
{
	Ogre::Real dist = (camera->getPosition() - point).length();

	camera->setPosition(point);

	if (horizontal)
	{
		camera->yaw(Ogre::Degree(-delta * 200.0f));
	}
	else
	{
		//camera->pitch(Ogre::Degree(-delta * 200.0f));

		Ogre::Matrix3 rotMat;
		Ogre::Radian yaw, pitch, roll;
		camera->getOrientation().ToRotationMatrix(rotMat);
		rotMat.ToEulerAnglesYXZ(yaw, pitch, roll);

		pitch += Ogre::Degree(-delta * 200.0f);

		if(pitch > Ogre::Degree(89.0)) pitch = Ogre::Degree(89.0f);
		if(pitch < Ogre::Degree(-89.0)) pitch = Ogre::Degree(-89.0f);

		rotMat.FromEulerAnglesYXZ(yaw, pitch, roll);

		camera->setOrientation(Ogre::Quaternion(rotMat));
	}

	camera->moveRelative(Ogre::Vector3(0, 0, dist));
}

//////////////////////////////////////////////////////////////////////////
void FocusCameraController::Zoom(Ogre::Camera* camera, const Ogre::Vector3& point, float delta)
{
	if (camera->getProjectionType() == Ogre::PT_ORTHOGRAPHIC)
	{
		ZoomOrthographic(delta);
		return;
	}

	Ogre::Real dist = (camera->getPosition() - point).length();

	float speed = dist * 2.0f;
	speed = std::max(speed, 200.0f) * delta;

	Ogre::Vector3 newPos = camera->getPosition() + camera->getOrientation() * Ogre::Vector3(0, 0, speed);

	Ogre::Vector3 origDir = camera->getPosition() - point;
	Ogre::Vector3 newDir = newPos - point;

	if (origDir.dotProduct(newDir) > 0.0f)
		camera->setPosition(newPos);
}

//////////////////////////////////////////////////////////////////////////
void FocusCameraController::Pan(Ogre::Camera* camera, float deltaX, float deltaY)
{
	Ogre::Vector3 point = m_FocusObject->getParentSceneNode()->_getDerivedPosition() + m_FocusOffset;
	Ogre::Real dist = std::max(100.0f, (camera->getPosition() - point).length());


	Ogre::Vector3 right = camera->getRight() * -deltaX * dist;
	Ogre::Vector3 up = camera->getUp() * deltaY * dist;

	camera->setPosition(camera->getPosition() + right + up);

	m_FocusOffset = m_FocusOffset + right + up;
	camera->setAutoTracking(true, m_FocusObject->getParentSceneNode(), m_FocusOffset);
}

//////////////////////////////////////////////////////////////////////////
void FocusCameraController::ZoomWheel(bool zoomIn)
{
	Ogre::Camera* camera = m_EditingCamera->GetOgreCamera();
	if (camera->getProjectionType() == Ogre::PT_ORTHOGRAPHIC)
	{
		ZoomOrthographic(zoomIn ? -0.1f : 0.1f);
		return;
	}
	
	if (m_IsDragging && m_FocusObject)
	{
		Ogre::Vector3 point = m_FocusObject->getParentSceneNode()->_getDerivedPosition() + m_FocusOffset;
		Zoom(camera, point, zoomIn ? -0.1f : 0.1f);
	}
	else
	{
		Ogre::Vector3 newPos = camera->getPosition() + camera->getOrientation() * Ogre::Vector3(0, 0, zoomIn ? -100.0f : 100.0f);
		camera->setPosition(newPos);
	}	
}

//////////////////////////////////////////////////////////////////////////
void FocusCameraController::ZoomOrthographic(float delta)
{
	Ogre::Camera* camera = m_EditingCamera->GetOgreCamera();

	Ogre::Real newHeight = camera->getOrthoWindowHeight() + 1000.0f * delta;
	if (newHeight < 0.0f) return;

	camera->setOrthoWindowHeight(newHeight);
}

//////////////////////////////////////////////////////////////////////////
void FocusCameraController::CenterMouse()
{
	m_Viewport->SetRelativeMousePos(0.5f, 0.5f);
	m_Viewport->GetRelativeMousePos(m_PrevMouseX, m_PrevMouseY);
}

//////////////////////////////////////////////////////////////////////////
bool FocusCameraController::GetFocusObject()
{
	if (!m_EditingCamera) return false;

	Stage* stage = m_EditingCamera->GetStage();
	if (!stage) return false;

	float x, y;
	m_Viewport->GetRelativeMousePos(x, y);


	Ogre::Ray mouseray = m_EditingCamera->GetOgreCamera()->getCameraToViewportRay(x, y);

	MousePickParams pickParams;
	pickParams.Point = mouseray.getPoint(0);
	pickParams.Normal = mouseray.getDirection();
	pickParams.RenderQueueId = -1;
	pickParams.QueryUV = false;


	if (stage->RaycastFromPoint(pickParams))
	{
		m_FocusObject = pickParams.ResultEntity;
		m_FocusOffset = pickParams.ResultPoint - pickParams.ResultEntity->getParentSceneNode()->_getDerivedPosition();

		return true;
	}
	else return false;
}


} // namespace Wme
