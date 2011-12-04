// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "LightEntity.h"
#include "LightGizmo.h"
#include "Scene3DBase.h"
#include "StringUtil.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
LightEntity::LightEntity()
{
	m_Light = NULL;

	m_IsGizmoVisible = false;
	m_Gizmo = NULL;

	m_LightType = LIGHT_POINT;

	m_DiffuseColor = Ogre::ColourValue::White;
	m_SpecularColor = Ogre::ColourValue::Black;

	m_Range = 100000;
	m_AttenuationConstant = 1.0f;
	m_AttenuationLinear = 0.0f;
	m_AttenuationQuadratic = 0.0f;

	m_SpotlightOuterAngle = Ogre::Degree(40.0f);
	m_SpotlightInnerAngle = Ogre::Degree(30.0f);
	m_SpotlightFalloff = 1.0f;

	m_CastShadows = true;
}

//////////////////////////////////////////////////////////////////////////
LightEntity::~LightEntity()
{
	SAFE_DELETE(m_Gizmo);
	RemoveFromStage();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity)
{
	RemoveFromStage();

	Entity3DBase::PutToStage(stage, parentEntity);

	WideString internalName = stage->GetUniqueMovableName("Light", L"Light");

	m_Light = stage->GetSceneMgr()->createLight(StringUtil::WideToUtf8(internalName));
	RebuildOgreLight();
	m_SceneNode->attachObject(m_Light);

	m_Stage->OnAddEntity(this);
	SetOwnedByStage(true);

	// temp
	SetGizmoVisible(true);
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::RemoveFromStage()
{
	Scene3DBase* stage = m_Stage;

	Entity3DBase::RemoveFromStage();

	if (m_Light)
	{
		if (stage) stage->GetSceneMgr()->destroyLight(m_Light);
		m_Light = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::PreRender(Viewport* viewport)
{
	if (m_Gizmo) m_Gizmo->PreRender(viewport);
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetGizmoVisible(bool visible)
{
	if (!IsInStage()) return;
	if (visible == m_IsGizmoVisible) return;

	if (visible)
	{
		m_Gizmo = new LightGizmo(this);
		m_Gizmo->Create();
	}
	else
	{
		SAFE_DELETE(m_Gizmo);
	}
	m_IsGizmoVisible = visible;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Entity* LightEntity::GetSelectableEntity() const
{
	if (m_Gizmo) return m_Gizmo->GetOgreEntity();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::RebuildOgreLight()
{
	if (!m_Light) return;

	switch (m_LightType)
	{
	case LIGHT_POINT:
		m_Light->setType(Ogre::Light::LT_POINT);
		break;
	case LIGHT_DIRECTIONAL:
		m_Light->setType(Ogre::Light::LT_DIRECTIONAL);
		break;
	case LIGHT_SPOT:
		m_Light->setType(Ogre::Light::LT_SPOTLIGHT);
		break;
	}

	m_Light->setDiffuseColour(m_DiffuseColor);
	m_Light->setSpecularColour(m_SpecularColor);

	m_Light->setAttenuation(m_Range, m_AttenuationConstant, m_AttenuationLinear, m_AttenuationQuadratic);

	if (m_LightType == LIGHT_SPOT)
		m_Light->setSpotlightRange(m_SpotlightInnerAngle, m_SpotlightOuterAngle, m_SpotlightFalloff);

	m_Light->setCastShadows(m_CastShadows);
	m_Light->setVisible(m_Active);
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetLightType(LightType type)
{
	m_LightType = type;
	RebuildOgreLight();

	if (m_Gizmo) m_Gizmo->Create();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetDiffuseColor(const Ogre::ColourValue& color)
{
	m_DiffuseColor = color;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetSpecularColor(const Ogre::ColourValue& color)
{
	m_SpecularColor = color;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetAttenuation(Ogre::Real range, Ogre::Real constant, Ogre::Real linear, Ogre::Real quadratic)
{
	m_Range = range;
	m_AttenuationConstant = constant;
	m_AttenuationLinear = linear;
	m_AttenuationQuadratic = quadratic;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetSpotlightRange(const Ogre::Radian& innerAngle, const Ogre::Radian& outerAngle, Ogre::Real falloff)
{
	m_SpotlightInnerAngle = innerAngle;
	m_SpotlightOuterAngle = outerAngle;
	m_SpotlightFalloff = falloff;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetSpotlightInnerAngle(const Ogre::Radian& val)
{
	m_SpotlightInnerAngle = val;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetSpotlightOuterAngle(const Ogre::Radian& val)
{
	m_SpotlightOuterAngle = val;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetSpotlightFalloff(Ogre::Real val)
{
	m_SpotlightFalloff = val;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetCastShadows(bool val)
{
	m_CastShadows = val;
	RebuildOgreLight();
}

//////////////////////////////////////////////////////////////////////////
void LightEntity::SetActive(bool val)
{
	m_Active = val;
	RebuildOgreLight();

	SetGizmoVisible(val);
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool LightEntity::LoadFromXml(TiXmlElement* rootNode)
{
	Entity3DBase::LoadFromXml(rootNode);

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Light")
		{
			for (TiXmlElement* lightElem = elem->FirstChildElement(); lightElem != NULL; lightElem = lightElem->NextSiblingElement())
			{
				if (lightElem->ValueStr() == "Type")
				{
					Utf8String strType = XmlUtil::TextToUtf8(lightElem);
					if (strType == "Point") m_LightType = LIGHT_POINT;
					else if (strType == "Directional") m_LightType = LIGHT_DIRECTIONAL;
					else if (strType == "Spot") m_LightType = LIGHT_SPOT;
				}
				else if (lightElem->ValueStr() == "DiffuseColor")
				{
					m_DiffuseColor = XmlUtil::GetColor(lightElem);
				}
				else if (lightElem->ValueStr() == "SpecularColor")
				{
					m_SpecularColor = XmlUtil::GetColor(lightElem);
				}
				else if (lightElem->ValueStr() == "Range")
				{
					m_Range = XmlUtil::TextToFloat(lightElem);
				}
				else if (lightElem->ValueStr() == "AttenuationConstant")
				{
					m_AttenuationConstant = XmlUtil::TextToFloat(lightElem);
				}
				else if (lightElem->ValueStr() == "AttenuationLinear")
				{
					m_AttenuationLinear = XmlUtil::TextToFloat(lightElem);
				}
				else if (lightElem->ValueStr() == "AttenuationQuadratic")
				{
					m_AttenuationQuadratic = XmlUtil::TextToFloat(lightElem);
				}
				else if (lightElem->ValueStr() == "SpotlightOuterAngle")
				{
					m_SpotlightOuterAngle = Ogre::Radian(Ogre::Degree(XmlUtil::TextToFloat(lightElem)));
				}
				else if (lightElem->ValueStr() == "SpotlightInnerAngle")
				{
					m_SpotlightInnerAngle = Ogre::Radian(Ogre::Degree(XmlUtil::TextToFloat(lightElem)));
				}
				else if (lightElem->ValueStr() == "SpotlightFalloff")
				{
					m_SpotlightFalloff = XmlUtil::TextToFloat(lightElem);
				}
				else if (lightElem->ValueStr() == "CastShadows")
				{
					m_CastShadows = XmlUtil::TextToBool(lightElem);
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool LightEntity::SaveToXml(TiXmlElement* rootNode)
{
	Entity3DBase::SaveToXml(rootNode);

	TiXmlElement* lightElem = XmlUtil::AddElem("Light", rootNode);

	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Type", lightElem);
	switch (m_LightType)
	{
	case LIGHT_POINT:
		XmlUtil::SetText(elem, "Point");
		break;
	case LIGHT_DIRECTIONAL:
		XmlUtil::SetText(elem, "Directional");
		break;
	case LIGHT_SPOT:
		XmlUtil::SetText(elem, "Spot");
		break;
	}
	
	elem = XmlUtil::AddElem("DiffuseColor", lightElem);
	XmlUtil::SetColor(elem, m_DiffuseColor);

	elem = XmlUtil::AddElem("SpecularColor", lightElem);
	XmlUtil::SetColor(elem, m_SpecularColor);

	elem = XmlUtil::AddElem("Range", lightElem);
	XmlUtil::SetText(elem, m_Range);

	elem = XmlUtil::AddElem("AttenuationConstant", lightElem);
	XmlUtil::SetText(elem, m_AttenuationConstant);

	elem = XmlUtil::AddElem("AttenuationLinear", lightElem);
	XmlUtil::SetText(elem, m_AttenuationLinear);

	elem = XmlUtil::AddElem("AttenuationQuadratic", lightElem);
	XmlUtil::SetText(elem, m_AttenuationQuadratic);

	elem = XmlUtil::AddElem("SpotlightOuterAngle", lightElem);
	XmlUtil::SetText(elem, m_SpotlightOuterAngle.valueDegrees());

	elem = XmlUtil::AddElem("SpotlightInnerAngle", lightElem);
	XmlUtil::SetText(elem, m_SpotlightInnerAngle.valueDegrees());

	elem = XmlUtil::AddElem("SpotlightFalloff", lightElem);
	XmlUtil::SetText(elem, m_SpotlightFalloff);

	elem = XmlUtil::AddElem("CastShadows", lightElem);
	XmlUtil::SetText(elem, m_CastShadows);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void LightEntity::RegisterScriptInterface()
{
	Entity3DBase::RegisterScriptInterface();
}


} // namespace Wme
