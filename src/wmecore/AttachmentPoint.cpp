// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AttachmentPoint.h"
#include "Entity3DBase.h"
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

	m_AttachedTo->GetSceneNode()->setListener(this);
	m_Bone->setListener(this);
}

//////////////////////////////////////////////////////////////////////////
AttachmentPoint::~AttachmentPoint()
{
	m_AttachedTo->GetSceneNode()->setListener(NULL);
	m_Bone->setListener(NULL);
}

//////////////////////////////////////////////////////////////////////////
void AttachmentPoint::Update()
{
	//m_Bone->needUpdate();
}

//////////////////////////////////////////////////////////////////////////
// Ogre::Node::Listener
//////////////////////////////////////////////////////////////////////////
void AttachmentPoint::nodeUpdated(const Ogre::Node* node)
{
	Ogre::Vector3 bonePos;
	Ogre::Quaternion boneRot;
	
	OgreUtil::GetBoneWorldPosition(m_AttachedTo->GetOgreEntity(), m_Bone, bonePos, boneRot);

	// test only
	m_Attachment->GetSceneNode()->_setDerivedPosition(bonePos);
	m_Attachment->GetSceneNode()->_setDerivedOrientation(boneRot);
}


} // namespace Wme
