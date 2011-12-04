// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "CameraEntity.h"
#include "Camera.h"
#include "CameraGizmo.h"
#include "Scene3DBase.h"
#include "StringUtil.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
CameraEntity::CameraEntity()
{
	m_Camera = NULL;

	m_IsGizmoVisible = false;
	m_Gizmo = NULL;

	m_ProjectionType = PROJECTION_PERSPECTIVE;
	m_FOVy = Ogre::Radian(Ogre::Math::PI / 4.0f);
	m_OrthoWindowHeight = 1000.0f;
	m_NearClipDistance = 100.0f; 
	m_FarClipDistance = 100000.0f;
	m_AspectRatio = 1.33333333333333f;
	m_CustomAspectRatio = false;
}

//////////////////////////////////////////////////////////////////////////
CameraEntity::~CameraEntity()
{
	SAFE_DELETE(m_Gizmo);
	RemoveFromStage();
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity)
{
	RemoveFromStage();

	Entity3DBase::PutToStage(stage, parentEntity);

	m_Camera = new Camera();
	m_Camera->Create(stage, m_Name);
	m_Camera->SetOwner(this);
	m_SceneNode->attachObject(m_Camera->GetOgreCamera());
	RebuildOgreCamera();

	m_Stage->OnAddEntity(this);
	SetOwnedByStage(true);

	// temp
	SetGizmoVisible(true);
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::RemoveFromStage()
{
	Entity3DBase::RemoveFromStage();
	SAFE_DELETE(m_Camera);
}


//////////////////////////////////////////////////////////////////////////
void CameraEntity::PreRender(Viewport* viewport)
{
	if (m_Gizmo) m_Gizmo->PreRender(viewport);
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetGizmoVisible(bool visible)
{
	if (visible == m_IsGizmoVisible) return;

	if (visible)
	{
		m_Gizmo = new CameraGizmo(this);
		m_Gizmo->Create();
	}
	else
	{
		SAFE_DELETE(m_Gizmo);
	}
	m_IsGizmoVisible = visible;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Camera* CameraEntity::GetOgreCamera() const
{
	if (m_Camera) return m_Camera->GetOgreCamera();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Entity* CameraEntity::GetSelectableEntity() const
{
	if (m_Gizmo) return m_Gizmo->GetOgreEntity();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::RebuildOgreCamera()
{
	if (!m_Camera) return;
	Ogre::Camera* cam = m_Camera->GetOgreCamera();
	if (!cam) return;

	switch (m_ProjectionType)
	{
	case PROJECTION_PERSPECTIVE:
		cam->setProjectionType(Ogre::PT_PERSPECTIVE);
		break;
	case PROJECTION_ORTHOGRAPHIC:
		cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		break;
	}

	cam->setFOVy(m_FOVy);
	cam->setNearClipDistance(m_NearClipDistance);
	cam->setFarClipDistance(m_FarClipDistance);
	cam->setOrthoWindowHeight(m_OrthoWindowHeight);

	if (m_CustomAspectRatio)
		cam->setAspectRatio(m_AspectRatio);

	m_Camera->SetCustomAspectRatio(m_CustomAspectRatio);
	
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetProjectionType(ProjectionType type)
{
	m_ProjectionType = type;
	RebuildOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetFOVy(const Ogre::Radian& fovy)
{
	m_FOVy = fovy;
	RebuildOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetNearClipDistance(Ogre::Real distance)
{
	m_NearClipDistance = distance;
	RebuildOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetFarClipDistance(Ogre::Real distance)
{
	m_FarClipDistance = distance;
	RebuildOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetAspectRatio(Ogre::Real ratio)
{
	m_AspectRatio = ratio;
	RebuildOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetCustomAspectRatio(bool customRatio)
{
	m_CustomAspectRatio = customRatio;
	RebuildOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
void CameraEntity::SetOrthoWindowHeight(Ogre::Real height)
{
	m_OrthoWindowHeight = height;
	RebuildOgreCamera();
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool CameraEntity::LoadFromXml(TiXmlElement* rootNode)
{
	Entity3DBase::LoadFromXml(rootNode);

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Camera")
		{
			for (TiXmlElement* camElem = elem->FirstChildElement(); camElem != NULL; camElem = camElem->NextSiblingElement())
			{
				if (camElem->ValueStr() == "ProjectionType")
				{
					Utf8String strType = XmlUtil::TextToUtf8(camElem);
					if (strType == "Perspective") m_ProjectionType = PROJECTION_PERSPECTIVE;
					else if (strType == "Orthographic") m_ProjectionType = PROJECTION_ORTHOGRAPHIC;
				}
				else if (camElem->ValueStr() == "FOVy")
				{
					m_FOVy = Ogre::Radian(Ogre::Degree(XmlUtil::TextToFloat(camElem)));
				}
				else if (camElem->ValueStr() == "FarClipDistance")
				{
					m_FarClipDistance = XmlUtil::TextToFloat(camElem);
				}
				else if (camElem->ValueStr() == "NearClipDistance")
				{
					m_NearClipDistance = XmlUtil::TextToFloat(camElem);
				}
				else if (camElem->ValueStr() == "AspectRatio")
				{
					m_AspectRatio = XmlUtil::TextToFloat(camElem);
				}
				else if (camElem->ValueStr() == "CustomAspectRatio")
				{
					m_CustomAspectRatio = XmlUtil::TextToBool(camElem);
				}
				else if (camElem->ValueStr() == "OrthoWindowHeight")
				{
					m_OrthoWindowHeight = XmlUtil::TextToFloat(camElem);
				}

			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CameraEntity::SaveToXml(TiXmlElement* rootNode)
{
	Entity3DBase::SaveToXml(rootNode);

	TiXmlElement* camElem = XmlUtil::AddElem("Camera", rootNode);

	TiXmlElement* elem;

	elem = XmlUtil::AddElem("ProjectionType", camElem);
	switch (m_ProjectionType)
	{
	case PROJECTION_PERSPECTIVE:
		XmlUtil::SetText(elem, "Perspective");
		break;
	case PROJECTION_ORTHOGRAPHIC:
		XmlUtil::SetText(elem, "Orthographic");
		break;
	}

	elem = XmlUtil::AddElem("FOVy", camElem);
	XmlUtil::SetText(elem, m_FOVy.valueDegrees());

	elem = XmlUtil::AddElem("FarClipDistance", camElem);
	XmlUtil::SetText(elem, m_FarClipDistance);

	elem = XmlUtil::AddElem("NearClipDistance", camElem);
	XmlUtil::SetText(elem, m_NearClipDistance);

	elem = XmlUtil::AddElem("AspectRatio", camElem);
	XmlUtil::SetText(elem, m_AspectRatio);

	elem = XmlUtil::AddElem("CustomAspectRatio", camElem);
	XmlUtil::SetText(elem, m_CustomAspectRatio);

	elem = XmlUtil::AddElem("OrthoWindowHeight", camElem);
	XmlUtil::SetText(elem, m_OrthoWindowHeight);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void CameraEntity::RegisterScriptInterface()
{
	Entity3DBase::RegisterScriptInterface();
}


} // namespace Wme
