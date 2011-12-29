// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "HelperEntity3D.h"
#include "HelperGizmo.h"
#include "Scene3DBase.h"
#include "StringUtil.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
HelperEntity3D::HelperEntity3D()
{
	m_IsGizmoVisible = false;
	m_Gizmo = NULL;
}

//////////////////////////////////////////////////////////////////////////
HelperEntity3D::~HelperEntity3D()
{
	SAFE_DELETE(m_Gizmo);
	RemoveFromStage();
}

//////////////////////////////////////////////////////////////////////////
void HelperEntity3D::PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity)
{
	RemoveFromStage();

	Entity3DBase::PutToStage(stage, parentEntity);

	m_Stage->OnAddEntity(this);
	SetOwnedByStage(true);

	// temp
	SetGizmoVisible(true);
}

//////////////////////////////////////////////////////////////////////////
void HelperEntity3D::PreRender(Viewport* viewport)
{
	if (m_Gizmo) m_Gizmo->PreRender(viewport);
}

//////////////////////////////////////////////////////////////////////////
void HelperEntity3D::SetGizmoVisible(bool visible)
{
	if (!IsInStage()) return;
	if (visible == m_IsGizmoVisible) return;

	if (visible)
	{
		m_Gizmo = new HelperGizmo(this);
		m_Gizmo->Create();
	}
	else
	{
		SAFE_DELETE(m_Gizmo);
	}
	m_IsGizmoVisible = visible;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Entity* HelperEntity3D::GetSelectableEntity() const
{
	if (m_Gizmo) return m_Gizmo->GetOgreEntity();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void HelperEntity3D::SetActive(bool val)
{
	m_Active = val;
	SetGizmoVisible(val);
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool HelperEntity3D::LoadFromXml(TiXmlElement* rootNode)
{
	Entity3DBase::LoadFromXml(rootNode);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool HelperEntity3D::SaveToXml(TiXmlElement* rootNode)
{
	Entity3DBase::SaveToXml(rootNode);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void HelperEntity3D::RegisterScriptInterface()
{
	Entity3DBase::RegisterScriptInterface();
}


} // namespace Wme
