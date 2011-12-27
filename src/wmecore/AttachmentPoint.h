// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAttachmentPoint_H__
#define __WmeAttachmentPoint_H__


#include "Object.h"


namespace Wme
{
	class Entity3DBase;
	class MeshEntity;
	class AttachmentPoint;

	
	typedef std::list<AttachmentPoint*> AttachmentList;

	
	class AttachmentPoint : public Object, public Ogre::Node::Listener
	{
	public:
		AttachmentPoint(Entity3DBase* attachment, MeshEntity* attachedTo, Ogre::Bone* bone);
		~AttachmentPoint();

		MeshEntity* GetAttachedTo() const { return m_AttachedTo; }
		Entity3DBase* GetAttachment() const { return m_Attachment; }
		Ogre::Bone* GetBone() const { return m_Bone; }
		Ogre::SceneNode* GetNode() const { return m_Node; }

		void Update();

		// Ogre::Node::Listener
		void nodeUpdated(const Ogre::Node* node);

	private:
		MeshEntity* m_AttachedTo;
		Entity3DBase* m_Attachment;
		Ogre::Bone* m_Bone;

		Ogre::SceneNode* m_Node;
	};
}

#endif // __WmeAttachmentPoint_H__
