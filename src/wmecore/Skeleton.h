// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSkeleton_H__
#define __WmeSkeleton_H__


#include "Object.h"
#include "RefCountObject.h"


namespace Wme
{
	class WmeDllExport Skeleton : public Object, public RefCountObject
	{
	public:
		Skeleton();
		~Skeleton();

		bool LoadFromFile(const WideString& fileName);

		WideString GetFileName() const { return m_FileName; }

		Ogre::SkeletonPtr GetOgreSkeleton() const { return m_OgreSkeleton; }

		bool AddSkeleton(const WideString& fileName, const WideString& animsToMerge = L"", bool recordChanges = true);
		bool DeleteAnimation(const WideString& animationName, bool recordChanges = true);

		void ReplayChanges();

	private:
		WideString m_FileName;
		Ogre::SkeletonPtr m_OgreSkeleton;

		static void GetSkeletonAnimList(Ogre::SkeletonPtr skeleton, const WideString& reqList, WideStringSet& animList);

		
		//////////////////////////////////////////////////////////////////////////
		class SkeletonChange
		{
		public:
			typedef std::set<WideString> AnimSet;

			SkeletonChange(const WideString& fileName, const AnimSet& animSet)
			{
				m_FileName = fileName;
				foreach (WideString str, animSet)
				{
					m_AnimSet.insert(str);
				}
			}

			bool DeleteAnim(const WideString& animName)
			{
				if (m_AnimSet.find(animName) != m_AnimSet.end())
				{
					m_AnimSet.erase(animName);
					return true;
				}
				else return false;
			}

			bool IsEmpty() const
			{
				return m_AnimSet.empty();
			}

			WideString GetFileName() const { return m_FileName; }
			WideString GetAnimList() const
			{
				WideString ret;
				foreach (WideString animName, m_AnimSet)
				{
					if (!ret.empty()) ret += L",";
					ret += animName;
				}
				return ret;
			}
		private:
			WideString m_FileName;
			AnimSet m_AnimSet;
		};


		typedef std::list<SkeletonChange*> ChangeList;
		ChangeList m_ChangeList;

		WideStringSet m_DeleteFromBase;
	};
}

#endif // __WmeSkeleton_H__
