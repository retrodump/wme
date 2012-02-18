// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMesh_H__
#define __WmeMesh_H__


#include "Object.h"
#include "RefCountObject.h"


namespace Wme
{
	class Skeleton;
	class MeshEntity;

	class WmeDllExport Mesh : public Object, public RefCountObject, public Ogre::MeshSerializerListener
	{
	public:
		Mesh();
		~Mesh();

		bool LoadFromFile(const WideString& fileName);

		WideString GetFileName() const { return m_FileName; }

		Ogre::MeshPtr GetOgreMesh() const { return static_cast<Ogre::MeshPtr>(m_OgreMesh); } // BAD

		void AddOwner(MeshEntity* entity);
		void RemoveOwner(MeshEntity* entity);
		bool IsOwnedBy(MeshEntity* entity) const;

		bool SupportsSkeletalAnimation() const;

		Skeleton* GetSkeleton() { return m_Skeleton; }
		bool SetSkeleton(const WideString& skeletonFile);
		bool AddSkeleton(const WideString& skeletonFile, const WideString& animsToMerge = L"");
		void RemoveSkeleton();
		bool DeleteAnimation(const WideString& animationName);
		WideString GetSkeletonFileName() const;
		bool HasAnimation(const WideString& animName) const;


		// Ogre::MeshSerializerListener
		void processMaterialName(Ogre::Mesh* mesh, Ogre::String* name);
		void processSkeletonName(Ogre::Mesh* mesh, Ogre::String* name);

	private:
		WideString m_FileName;
		Ogre::SkeletonPtr m_OgreMesh;

		Skeleton* m_Skeleton;

		typedef std::set<MeshEntity*> MeshEntityCollection;
		MeshEntityCollection m_Owners;

	};
}

#endif // __WmeMesh_H__
