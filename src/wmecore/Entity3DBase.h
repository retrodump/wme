// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEntity3DBase_H__
#define __WmeEntity3DBase_H__


#include "Entity.h"


namespace Wme
{
	class Scene3DBase;
	class AttachmentPoint;

	//////////////////////////////////////////////////////////////////////////
	/** Abstraction of a 3D object that can be added to a scene graph or attached to a bone */
	class WmeDllExport Entity3DBase : public Entity
	{
	public:
		Entity3DBase();
		virtual ~Entity3DBase();

		bool IsInStage() const  { return m_Stage != NULL; }
		Scene3DBase* GetStage() const { return m_Stage; }

		virtual void PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity = NULL);
		virtual void RemoveFromStage();


		/// if the entity isn't attached to root node, it will be detached and attached to root
		/// this is used during scene destruction to keep persistent entities around
		void DetachFromParentNode();

		void SetPosition(const Ogre::Vector3& pos);
		const Ogre::Vector3& GetPosition() const;

		void SetOrientation(const Ogre::Quaternion& orientation);
		const Ogre::Quaternion& GetOrientation() const;

		void SetScale(const Ogre::Vector3& scale);
		const Ogre::Vector3& GetScale() const;

		bool IsOwnedByStage() const { return m_IsOwnedByStage; }
		void SetOwnedByStage(bool val) { m_IsOwnedByStage = val; }

		Entity3DBase* GetParentEntity() const;
		word GetNumChildren() const;
		Entity3DBase* GetChildEntity(word index) const;

		Ogre::SceneNode* GetSceneNode() const { return m_SceneNode; }

		AttachmentPoint* GetAttachedTo() const { return m_AttachedTo; }
		void SetAttachedTo(AttachmentPoint* attachTo);
		void DetachIfAttached();


		// editor support
		virtual bool IsEditorMovable() const { return true; }
		virtual bool IsEditorRotatable() const { return true; }
		virtual bool IsEditorScalable() const { return false; }
		virtual bool IsMeshBased() const { return false; }
		virtual Ogre::Entity* GetSelectableEntity() const { return NULL; }

		// conversion helpers
		static Entity3DBase* NodeToEntity(Ogre::SceneNode* node);
		static Entity3DBase* OgreEntityToEntity(Ogre::Entity* entity);

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);


	protected:
		Scene3DBase* m_Stage;
		bool m_IsOwnedByStage;		
		Ogre::SceneNode* m_SceneNode;

		AttachmentPoint* m_AttachedTo;

		mutable Ogre::Vector3 m_Position;
		mutable Ogre::Quaternion m_Orientation;
		mutable Ogre::Vector3 m_Scale;
	};
}


#endif // __WmeEntity3DBase_H__
