// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMeshEntity_H__
#define __WmeMeshEntity_H__


#include "Entity3DBase.h"
#include "AnimProp.h"
#include "AttachmentPoint.h"


namespace Wme
{
	class Scene3DBase;
	class Mesh;
	class Skeleton;
	class Animation;
	class SimpleAnimTree;
	class MaterialInstance;
	

	//////////////////////////////////////////////////////////////////////////
	/** Abstraction of a mesh-based 3D object */
	class WmeDllExport MeshEntity : public Entity3DBase, public Ogre::MovableObject::Listener
	{
	public:
		MeshEntity();
		virtual ~MeshEntity();

		void Create(const WideString& meshFile, const WideString& skeletonFile);

		virtual void Update();

		void PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity = NULL);
		void RemoveFromStage();

		bool SetSkeleton(const WideString& skeletonFile, bool highlightMat = false);
		bool AddSkeleton(const WideString& skeletonFile, const WideString& animsToMerge = L"");
		void RemoveSkeleton(bool highlightMat = false);
		bool DeleteAnimation(const WideString& animationName);
		Skeleton* GetSkeleton() const;

		bool SetMesh(const WideString& meshFile, bool highlightMat = false);
		void RemoveMesh();

		void StartAnimation(const AnimProp& animProp);
		void StopAnimation(const AnimProp& animProp);
		bool IsAnimationPlaying(const AnimProp& animProp, bool includeFadingIn = true);
		bool IsAnyAnimationPlaying();

		virtual bool IsAnimFrozen() const;
		virtual bool IsChildSelectedInEditor() const;

		bool HasAnimation(const WideString& animName) const;

		void SetName(const WideString& name);

		Ogre::Entity* GetOgreEntity() { return m_OgreEntity; }
		const Ogre::Entity* GetOgreEntity() const { return m_OgreEntity; }

		void SetMaterial(const WideString& fileName, int subEntity = -1);
		MaterialInstance* GetMaterial(size_t subEntity);
		void ReApplyMaterial(size_t subEntity);
		void ReApplyAllMaterials();

		bool GetCastShadows() const { return m_CastShadows; }
		void SetCastShadows(bool val);

		WideString GetMeshFileName() const;
		WideString GetSkeletonFileName() const;

		bool SupportsSkeletalAnimation() const;


		// bone attachments
		void AddAttachment(Entity3DBase* entity, const WideString& boneName);
		void RemoveAttachment(Entity3DBase* entity);
		void RemoveAttachmentsFromBone(const WideString& boneName);
		void RemoveAllAttachments();

		void GetAttachmentsForBone(const WideString& boneName, AttachmentList& attachments) const;


		// editor support
		virtual bool IsEditorScalable() const { return true; }
		virtual bool IsMeshBased() const { return true; }
		virtual Ogre::Entity* GetSelectableEntity() const { return m_OgreEntity; }

		virtual void OnPreFindVisibleObjects();

		// Ogre::MovableObject::Listener
		void objectDestroyed(Ogre::MovableObject* obj);

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeEntity3D"; }

		// script interface
		virtual void RegisterScriptInterface();

		bool ScAddSkeleton(Script* script, const WideString& methodName, bool async);
		bool ScDeleteAnim(Script* script, const WideString& methodName, bool async);
		bool ScHasAnim(Script* script, const WideString& methodName, bool async);

		void OnSkeletonAdded();
		void OnSkeletonRemoved();

	protected:
		Ogre::Entity* m_OgreEntity;
		Mesh* m_Mesh;

		bool m_CastShadows;

		void BuildAnimTree();
		void KillAnimations();

		void RebuildOgreEntity();
		void ScheduleOgreEntityRebuild();

		void EmitNodeChanged();

		SimpleAnimTree* m_AnimTree;

		typedef std::map<WideString, Animation*> AnimationMap;
		AnimationMap m_Animations;

		typedef std::map<size_t, MaterialInstance*> MaterialInstanceMap;
		MaterialInstanceMap m_Materials;

	private:
		void ApplyMaterialInternal(size_t subEntity, bool highlighted);

		bool LoadMaterialsFromXml(TiXmlElement* rootNode);
		bool SaveMaterialsToXml(TiXmlElement* rootNode);

		bool m_HighlightMatAfterChange;

		typedef std::multimap<WideString, AttachmentPoint*> AttachmentMap;
		AttachmentMap m_Attachments;
	};
}

#endif // __WmeMeshEntity_H__
