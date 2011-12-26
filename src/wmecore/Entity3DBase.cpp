// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Entity3DBase.h"
#include "Scene3DBase.h"
#include "StringUtil.h"
#include "XmlUtil.h"
#include "AttachmentPoint.h"
#include "MeshEntity.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Entity3DBase::Entity3DBase()
{
	m_SceneNode = NULL;
	m_Stage = NULL;
	m_IsOwnedByStage = false;
	m_AttachedTo = NULL;

	m_Position = Ogre::Vector3::ZERO;
	m_Orientation = Ogre::Quaternion::IDENTITY;
	m_Scale = Ogre::Vector3::UNIT_SCALE;
}

//////////////////////////////////////////////////////////////////////////
Entity3DBase::~Entity3DBase()
{
	RemoveFromStage();
}

//////////////////////////////////////////////////////////////////////////
void Entity3DBase::PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity)
{
	RemoveFromStage();

	Utf8String nodeName = StringUtil::WideToUtf8(stage->GetUniqueNodeName(L"SceneNode"));

	if (parentEntity && parentEntity->GetSceneNode())
		m_SceneNode = parentEntity->GetSceneNode()->createChildSceneNode(nodeName);
	else
		m_SceneNode = stage->GetRootNode()->createChildSceneNode(nodeName);

	m_SceneNode->getUserObjectBindings().setUserAny("Entity3DBase", Ogre::Any(this));

	SetPosition(m_Position);
	SetScale(m_Scale);
	SetOrientation(m_Orientation);

	stage->OnSceneNodeCreated(m_SceneNode);
	m_Stage = stage;
}

//////////////////////////////////////////////////////////////////////////
void Entity3DBase::RemoveFromStage()
{
	DetachIfAttached();

	if (m_SceneNode)
	{
		m_Position = GetPosition();
		m_Orientation = GetOrientation();
		m_Scale = GetScale();

		if (m_Stage) m_Stage->OnRemoveEntity(this);
		m_SceneNode->detachAllObjects();

		if (m_Stage) m_Stage->OnSceneNodeDestroyed(m_SceneNode);
		if (m_SceneNode) m_SceneNode->getCreator()->destroySceneNode(m_SceneNode);
	}	
	
	m_SceneNode = NULL;
	m_Stage = NULL;
}

//////////////////////////////////////////////////////////////////////////
void Entity3DBase::DetachFromParentNode()
{
	if (m_SceneNode && m_Stage)
	{
		if (m_SceneNode->getParentSceneNode() != m_Stage->GetRootNode())
		{
			m_SceneNode->getParentSceneNode()->removeChild(m_SceneNode);
			m_Stage->GetRootNode()->addChild(m_SceneNode);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Entity3DBase* Entity3DBase::GetParentEntity() const
{
	if (!m_SceneNode) return NULL;
	
	Ogre::SceneNode* parentNode = m_SceneNode->getParentSceneNode();
	return NodeToEntity(parentNode);
}

//////////////////////////////////////////////////////////////////////////
word Entity3DBase::GetNumChildren() const
{
	if (!m_SceneNode) return 0;
	else return m_SceneNode->numChildren();
}

//////////////////////////////////////////////////////////////////////////
Entity3DBase* Entity3DBase::GetChildEntity(word index) const
{
	if (!m_SceneNode) return NULL;
	
	Ogre::SceneNode* childNode = static_cast<Ogre::SceneNode*>(m_SceneNode->getChild(index));
	return NodeToEntity(childNode);
}

//////////////////////////////////////////////////////////////////////////
Entity3DBase* Entity3DBase::NodeToEntity(Ogre::SceneNode* node)
{
	if (!node) return NULL;

	if (!node->getUserObjectBindings().getUserAny("Entity3DBase").isEmpty())
	{
		return Ogre::any_cast<Entity3DBase*>(node->getUserObjectBindings().getUserAny("Entity3DBase"));
	}
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
Entity3DBase* Entity3DBase::OgreEntityToEntity(Ogre::Entity* entity)
{
	if (!entity) return NULL;

	if (!entity->getUserObjectBindings().getUserAny("pickable").isEmpty())
	{
		PickableObject pickObj = Ogre::any_cast<PickableObject>(entity->getUserObjectBindings().getUserAny("pickable"));
		if (pickObj.GetType() == PickableObject::PICKABLE_ENTITY) return static_cast<Entity3DBase*>(pickObj.GetObject());
		else return NULL;
	}
	else return NULL;
}


//////////////////////////////////////////////////////////////////////////
void Entity3DBase::SetPosition(const Ogre::Vector3& pos)
{
	m_Position = pos;
	if (m_SceneNode) m_SceneNode->setPosition(pos);
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& Entity3DBase::GetPosition() const
{
	if (m_SceneNode) return m_SceneNode->getPosition();
	else return m_Position;
}

//////////////////////////////////////////////////////////////////////////
void Entity3DBase::SetOrientation(const Ogre::Quaternion& orientation)
{
	m_Orientation = orientation;
	if (m_SceneNode) m_SceneNode->setOrientation(orientation);
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Quaternion& Entity3DBase::GetOrientation() const
{
	if (m_SceneNode) return m_SceneNode->getOrientation();
	else return m_Orientation;
}

//////////////////////////////////////////////////////////////////////////
void Entity3DBase::SetScale(const Ogre::Vector3& scale)
{
	m_Scale = scale;
	if (m_SceneNode) m_SceneNode->setScale(scale);
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& Entity3DBase::GetScale() const
{
	if (m_SceneNode) return m_SceneNode->getScale();
	else return m_Scale;
}

//////////////////////////////////////////////////////////////////////////
void Entity3DBase::DetachIfAttached()
{
	if (m_AttachedTo)
	{
		m_AttachedTo->GetAttachedTo()->RemoveAttachment(this);
		SetAttachedTo(NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
void Entity3DBase::SetAttachedTo(AttachmentPoint* attachTo)
{
	m_AttachedTo = attachTo;
}

//////////////////////////////////////////////////////////////////////////
// DocumentAwareObject
//////////////////////////////////////////////////////////////////////////
bool Entity3DBase::LoadFromXml(TiXmlElement* rootNode)
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
					m_Position = XmlUtil::GetVector3(placeElem);
				}
				else if (placeElem->ValueStr() == "Orientation")
				{
					m_Orientation = XmlUtil::GetQuaternion(placeElem);
				}
				else if (placeElem->ValueStr() == "Scale")
				{
					m_Scale = XmlUtil::GetVector3(placeElem);
				}
			}
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Entity3DBase::SaveToXml(TiXmlElement* rootNode)
{
	Entity::SaveToXml(rootNode);

	TiXmlElement* placeElem = XmlUtil::AddElem("Placement", rootNode);

	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Position", placeElem);
	XmlUtil::SetVector3(elem, GetPosition());

	elem = XmlUtil::AddElem("Orientation", placeElem);
	XmlUtil::SetQuaternion(elem, GetOrientation());

	elem = XmlUtil::AddElem("Scale", placeElem);
	XmlUtil::SetVector3(elem, GetScale());


	return true;
}


} // namespace Wme
