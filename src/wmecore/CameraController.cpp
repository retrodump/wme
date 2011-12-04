// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "CameraController.h"
#include "Viewport.h"
#include "Camera.h"
#include "CameraEntity.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
CameraController::CameraController()
{
	m_Viewport = NULL;
	m_EditingCamera = NULL;
}

//////////////////////////////////////////////////////////////////////////
CameraController::~CameraController()
{
	SAFE_DELETE(m_EditingCamera);
}

//////////////////////////////////////////////////////////////////////////
void CameraController::Create(Viewport* viewport)
{
	m_Viewport = viewport;
	m_EditingCamera = new Camera();
	m_EditingCamera->Create(viewport->GetCamera()->GetStage(), L"sys/editing_camera");

	CloneCamera(viewport->GetCamera());
}

//////////////////////////////////////////////////////////////////////////
void CameraController::CloneCamera(Camera* camera)
{
	if (camera)
	{
		m_EditingCamera->GetOgreCamera()->synchroniseBaseSettingsWith(camera->GetOgreCamera());
		if (camera->GetOgreCamera()->isAttached())
		{
			m_EditingCamera->GetOgreCamera()->setPosition(camera->GetOgreCamera()->getRealPosition());
			m_EditingCamera->GetOgreCamera()->setOrientation(camera->GetOgreCamera()->getRealOrientation());
		}
		m_EditingCamera->SetCustomAspectRatio(camera->HasCustomAspectRatio());
		m_EditingCamera->SetStage(camera->GetStage());
	}
	//m_EditingCamera->GetOgreCamera()->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);

	if (m_Viewport) m_Viewport->SetCamera(m_EditingCamera);
}

//////////////////////////////////////////////////////////////////////////
void CameraController::SaveCameraTo(CameraEntity* targetCamera)
{
	targetCamera->GetOgreCamera()->synchroniseBaseSettingsWith(m_EditingCamera->GetOgreCamera());
	targetCamera->SetPosition(m_EditingCamera->GetOgreCamera()->getPosition());
	targetCamera->SetOrientation(m_EditingCamera->GetOgreCamera()->getOrientation());
	/*
	if (targetCamera->GetOgreCamera()->isAttached())
	{
		targetCamera->GetOgreCamera()->setPosition(Ogre::Vector3::ZERO);
		targetCamera->GetOgreCamera()->setOrientation(Ogre::Quaternion::IDENTITY);

		Ogre::SceneNode* parentNode = targetCamera->GetOgreCamera()->getParentSceneNode();
		parentNode->_setDerivedPosition(m_EditingCamera->GetOgreCamera()->getPosition());
		parentNode->_setDerivedOrientation(m_EditingCamera->GetOgreCamera()->getOrientation());
	}
	*/
}


} // namespace Wme
