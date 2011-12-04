// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Camera.h"
#include "CameraGizmo.h"
#include "Scene3DBase.h"
#include "StringUtil.h"
#include "ContentManager.h"


namespace Wme
{

size_t Camera::s_CameraCount = 0;

//////////////////////////////////////////////////////////////////////////
Camera::Camera()
{
	m_Stage = NULL;
	m_Owner = NULL;

	m_OgreCamera = NULL;
	m_OgreCameraOwned = false;

	m_CustomAspectRatio = false;
}

//////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{
	Game::GetInstance()->GetContentMgr()->OnCameraDestroyed(this);
	DestroyOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
void Camera::Create(Stage* stage, const WideString& camName)
{
	DestroyOgreCamera();

	m_Stage = stage;

	Ogre::String camNameStr;
	if (camName.empty()) camNameStr = "camera" + Ogre::StringConverter::toString(s_CameraCount++);
	else
	{
		WideString safeName = stage->GetUniqueMovableName("Camera", camName);
		camNameStr = StringUtil::WideToUtf8(safeName);
	}

	m_OgreCamera = m_Stage->GetSceneMgr()->createCamera(camNameStr);
	m_OgreCameraOwned = true;
}

//////////////////////////////////////////////////////////////////////////
void Camera::CreateFromOgreCamera(Stage* stage, Ogre::Camera* camera)
{
	DestroyOgreCamera();

	m_Stage = stage;
	m_OgreCamera = camera;
	m_OgreCameraOwned = false;
}

//////////////////////////////////////////////////////////////////////////
void Camera::DestroyOgreCamera()
{
	if (m_OgreCameraOwned)
	{
		if (m_Stage) m_Stage->_DestroyOgreCamera(this);
	}
	m_OgreCamera = NULL;
	m_OgreCameraOwned = false;
}


} // namespace Wme
