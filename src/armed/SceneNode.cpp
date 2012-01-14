// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "SceneNode.h"
#include "SceneNodeModel.h"
#include "Scene3DBase.h"
#include "Entity3DBase.h"
#include "QtUtil.h"
#include "StringUtil.h"
#include "SceneNodeIconCache.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
SceneNode::SceneNode(SceneNodeModel* parentModel, Ogre::SceneNode* node, SceneNode* parent)
{
	m_ParentModel = parentModel;

	m_Parent = parent;
	m_GenericNode = node;
	m_Type = NODE_GENERIC;	

	m_Scene = NULL;
	m_Entity = NULL;
	m_Bone = NULL;
	m_BoneOwner = NULL;

	if (m_Parent) m_Parent->AddChild(this);

	m_ParentModel->RegisterNode(this, GetOgreSceneNode());
}

//////////////////////////////////////////////////////////////////////////
SceneNode::SceneNode(SceneNodeModel* parentModel, Scene3DBase* scene, SceneNode* parent)
{
	m_ParentModel = parentModel;

	m_Parent = parent;
	m_Scene = scene;
	m_Type = NODE_SCENE;	

	m_GenericNode = NULL;
	m_Entity = NULL;
	m_Bone = NULL;
	m_BoneOwner = NULL;

	if (m_Parent) m_Parent->AddChild(this);

	m_ParentModel->RegisterNode(this, GetOgreSceneNode());
}

//////////////////////////////////////////////////////////////////////////
SceneNode::SceneNode(SceneNodeModel* parentModel, Entity3DBase* entity, SceneNode* parent)
{
	m_ParentModel = parentModel;

	m_Parent = parent;
	m_Entity = entity;
	m_Type = NODE_ENTITY;	

	m_GenericNode = NULL;
	m_Scene = NULL;
	m_Bone = NULL;
	m_BoneOwner = NULL;

	if (m_Parent) m_Parent->AddChild(this);

	m_ParentModel->RegisterNode(this, GetOgreSceneNode());
}

//////////////////////////////////////////////////////////////////////////
SceneNode::SceneNode(SceneNodeModel* parentModel, MeshEntity* boneOwner, Ogre::Bone* bone, SceneNode* parent)
{
	m_ParentModel = parentModel;

	m_Parent = parent;
	m_Bone = bone;
	m_BoneOwner = boneOwner;
	m_Type = NODE_BONE;	

	m_GenericNode = NULL;
	m_Scene = NULL;
	m_Entity = NULL;

	if (m_Parent) m_Parent->AddChild(this);

	m_ParentModel->RegisterNode(this, GetOgreSceneNode());
}

//////////////////////////////////////////////////////////////////////////
SceneNode::~SceneNode()
{
	m_ParentModel->UnregisterNode(GetOgreSceneNode());
	RemoveChildren();
}

//////////////////////////////////////////////////////////////////////////
void SceneNode::AddChild(SceneNode* child)
{
	m_Children.append(child);
	child->SetParent(this);

	Ogre::SceneNode* ogreSceneNode = child->GetOgreSceneNode();
	if (ogreSceneNode) m_SceneNodeLookup[ogreSceneNode] = child;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode::RemoveChild(SceneNode* child)
{
	Ogre::SceneNode* ogreSceneNode = child->GetOgreSceneNode();
	if (m_SceneNodeLookup.contains(ogreSceneNode)) m_SceneNodeLookup.remove(ogreSceneNode);

	m_Children.removeOne(child);
	child->SetParent(NULL);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode::RemoveChildren()
{
	qDeleteAll(m_Children);
	m_Children.clear();
}

//////////////////////////////////////////////////////////////////////////
SceneNode* SceneNode::GetChildFromOgreSceneNode(Ogre::SceneNode* ogreSceneNode) const
{
	if (m_SceneNodeLookup.contains(ogreSceneNode)) return m_SceneNodeLookup[ogreSceneNode];
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
Ogre::SceneNode* SceneNode::GetOgreSceneNode() const
{
	switch (m_Type)
	{
	case NODE_SCENE:
		return GetScene()->GetRootNode();
		break;
	case NODE_GENERIC:
		return GetGenericNode();
	case NODE_ENTITY:
		return GetEntity()->GetSceneNode();
	default:
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
SceneNode* SceneNode::GetChildAt(int index) const
{
	if (m_Children.count() > index) return m_Children[index];
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
QString SceneNode::GetName() const
{
	switch (m_Type)
	{
	case NODE_ENTITY:
		return QtUtil::WideToQString(m_Entity->GetName());
	case NODE_SCENE:
		return QtUtil::WideToQString(m_Scene->GetName());
	case NODE_BONE:
		return QtUtil::WideToQString(StringUtil::Utf8ToWide(m_Bone->getName()));
	case NODE_GENERIC:
		return QtUtil::WideToQString(StringUtil::Utf8ToWide(m_GenericNode->getName()));
	}
	return QString();
}

//////////////////////////////////////////////////////////////////////////
QString SceneNode::GetTypeName() const
{
	switch (m_Type)
	{
	case NODE_ENTITY:
	{
		QString entityType, entityIcon;
		GetEntityTypeAndIcon(m_Entity, entityType, entityIcon);
		return entityType;
	}		
	case NODE_SCENE:
		return qApp->translate("SceneNode", "Scene");
	case NODE_BONE:
		return qApp->translate("SceneNode", "Bone");
	case NODE_GENERIC:
		return qApp->translate("SceneNode", "Node");
	}
	return QString();
}

//////////////////////////////////////////////////////////////////////////
void SceneNode::GetEntityTypeAndIcon(Entity3DBase* entity, QString& typeName, QString& iconName) const
{
	if (entity->GetEntitySubtype() == L"Light")
	{
		typeName = qApp->translate("SceneNode", "Light");
		iconName = ":/icons/lightbulb.png";
	}
	else if (entity->GetEntitySubtype() == L"Camera")
	{
		typeName = qApp->translate("SceneNode", "Camera");
		iconName = ":/icons/camera.png";
	}
	else if (entity->GetEntitySubtype() == L"Actor3D")
	{
		typeName = qApp->translate("SceneNode", "Actor");
		iconName = ":/icons/user.png";
	}
	else if (entity->GetEntitySubtype() == L"Helper3D")
	{
		typeName = qApp->translate("SceneNode", "Helper");
		iconName = ":/icons/chart_organisation.png";
	}
	else
	{
		typeName = qApp->translate("SceneNode", "Entity");
		iconName = ":/icons/status_offline.png";
	}
}

//////////////////////////////////////////////////////////////////////////
QIcon* SceneNode::GetIcon() const
{
	QString iconFile;

	switch (m_Type)
	{
	case NODE_ENTITY:
	{
		QString entityType;
		GetEntityTypeAndIcon(m_Entity, entityType, iconFile);
		break;
	}		
	case NODE_SCENE:
		iconFile = ":/icons/house.png";
		break;
	case NODE_BONE:
		iconFile = ":/icons/vector.png";
		break;
	case NODE_GENERIC:
		iconFile = ":/icons/chart_organisation.png";
		break;
	}

	if (!iconFile.isEmpty()) return SceneNodeIconCache::GetInstance()->GetIcon(iconFile);
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode::RegisterNode(SceneNodeModel* parentModel)
{
	parentModel->RegisterNode(this, GetOgreSceneNode());
	qforeach (SceneNode* child, m_Children)
	{
		child->RegisterNode(parentModel);
	}
}


} // namespace Armed
