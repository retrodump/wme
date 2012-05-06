// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Entity2DBase.h"
#include "Canvas2D.h"
#include "Scene2DBase.h"
#include "SceneNode2D.h"
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Entity2DBase::Entity2DBase()
{
	m_Stage = NULL;
	m_SceneNode = NULL;

	m_Position = Ogre::Vector2::ZERO;
	m_Rotation = 0;
	m_Scale = Ogre::Vector2::UNIT_SCALE;
}

//////////////////////////////////////////////////////////////////////////
Entity2DBase::~Entity2DBase()
{
	RemoveFromStage();
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::PutToStage(Scene2DBase* stage, Entity2DBase* parentEntity)
{
	RemoveFromStage();

	if (parentEntity && parentEntity->GetSceneNode())
		m_SceneNode = parentEntity->GetSceneNode()->CreateChildNode();
	else
		m_SceneNode = stage->GetCanvas()->GetRootNode()->CreateChildNode();

	SetPosition(m_Position);
	SetScale(m_Scale);
	SetRotation(m_Rotation);

	stage->OnSceneNodeCreated(m_SceneNode);
	m_Stage = stage;
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::RemoveFromStage()
{
	if (m_SceneNode)
	{
		m_Position = GetPosition();
		m_Rotation = GetRotation();
		m_Scale = GetScale();

		if (m_Stage) m_Stage->OnRemoveEntity(this);
		m_SceneNode->DetachElement();

		if (m_Stage) m_Stage->OnSceneNodeDestroyed(m_SceneNode);
		if (m_SceneNode)
		{
			if (m_SceneNode->GetParentNode()) m_SceneNode->GetParentNode()->RemoveChild(m_SceneNode);
			delete m_SceneNode;
		}
	}	

	m_SceneNode = NULL;
	m_Stage = NULL;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector2& Entity2DBase::GetPosition() const
{
	if (m_SceneNode) return m_SceneNode->GetPosition();
	else return m_Position;
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::SetPosition(const Ogre::Vector2& pos)
{
	m_Position = pos;
	if (m_SceneNode) m_SceneNode->SetPosition(pos);
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::SetPosition(float x, float y)
{
	SetPosition(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Degree& Entity2DBase::GetRotation() const
{
	if (m_SceneNode) return m_SceneNode->GetRotation();
	else return m_Rotation;
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::SetRotation(const Ogre::Degree& angle)
{
	m_Rotation = angle;
	if (m_SceneNode) m_SceneNode->SetRotation(angle);
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::SetRotation(float degrees)
{
	SetRotation(Ogre::Degree(degrees));
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector2& Entity2DBase::GetScale() const
{
	if (m_SceneNode) return m_SceneNode->GetScale();
	else return m_Scale;
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::SetScale(const Ogre::Vector2& scale)
{
	m_Scale = scale;
	if (m_SceneNode) m_SceneNode->SetScale(scale);
}

//////////////////////////////////////////////////////////////////////////
void Entity2DBase::SetScale(float x, float y)
{
	SetScale(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool Entity2DBase::LoadFromXml(TiXmlElement* rootNode)
{
	Entity::LoadFromXml(rootNode);

	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Placement")
		{
			for (TiXmlElement* placeElem = elem->FirstChildElement(); placeElem != NULL; placeElem = placeElem->NextSiblingElement())
			{
				if (placeElem->ValueStr() == "Position")
				{
					m_Position = XmlUtil::GetVector2(placeElem);
				}
				else if (placeElem->ValueStr() == "Rotation")
				{
					m_Rotation = Ogre::Degree(XmlUtil::TextToFloat(placeElem));
				}
				else if (placeElem->ValueStr() == "Scale")
				{
					m_Scale = XmlUtil::GetVector2(placeElem);
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Entity2DBase::SaveToXml(TiXmlElement* rootNode)
{
	Entity::SaveToXml(rootNode);

	TiXmlElement* placeElem = XmlUtil::AddElem("Placement", rootNode);

	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Position", placeElem);
	XmlUtil::SetVector2(elem, GetPosition());

	elem = XmlUtil::AddElem("Rotation", placeElem);
	XmlUtil::SetText(elem, GetRotation().valueDegrees());

	elem = XmlUtil::AddElem("Scale", placeElem);
	XmlUtil::SetVector2(elem, GetScale());


	return true;
}


} // namespace Wme
