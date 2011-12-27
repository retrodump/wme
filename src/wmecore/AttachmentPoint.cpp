// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AttachmentPoint.h"
#include "Entity3DBase.h"
#include "Scene3DBase.h"
#include "MeshEntity.h"
#include "OgreUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AttachmentPoint::AttachmentPoint(Entity3DBase* attachment, MeshEntity* attachedTo, Ogre::Bone* bone)
{
	m_Attachment = attachment;
	m_AttachedTo = attachedTo;
	m_Bone = bone;

	Scene3DBase* stage = attachedTo->GetStage();
	if (stage)
	{
		// create a new scene node and attach the attachment to it
		m_Node = stage->GetSystemRootNode()->createChildSceneNode();

		m_Attachment->GetSceneNode()->getParentSceneNode()->removeChild(m_Attachment->GetSceneNode());
		m_Node->addChild(m_Attachment->GetSceneNode());

		m_Attachment->SetPosition(Ogre::Vector3::ZERO);
		m_Attachment->SetOrientation(Ogre::Quaternion::IDENTITY);

		// setup a listener to track the entity and bone transform changes
		m_AttachedTo->GetSceneNode()->setListener(this);
		m_Bone->setListener(this);
	}
	else m_Node = NULL;

}

//////////////////////////////////////////////////////////////////////////
AttachmentPoint::~AttachmentPoint()
{
	m_AttachedTo->GetSceneNode()->setListener(NULL);
	m_Bone->setListener(NULL);

	if (m_Node)
	{
		m_Node->removeAllChildren();
		m_Node->getCreator()->destroySceneNode(m_Node);
	}
}

//////////////////////////////////////////////////////////////////////////
void AttachmentPoint::Update()
{
}

//////////////////////////////////////////////////////////////////////////
// Ogre::Node::Listener
//////////////////////////////////////////////////////////////////////////
void AttachmentPoint::nodeUpdated(const Ogre::Node* node)
{
	if (!m_Node) return;

	Ogre::Vector3 bonePos;
	Ogre::Quaternion boneRot;
	
	OgreUtil::GetBoneWorldPosition(m_AttachedTo->GetOgreEntity(), m_Bone, bonePos, boneRot);
	
	m_Node->_setDerivedPosition(bonePos);
	m_Node->_setDerivedOrientation(boneRot);
	m_Node->_update(true, true);
}


} // namespace Wme
