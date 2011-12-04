// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSkeletonManager_H__
#define __WmeSkeletonManager_H__


#include "Object.h"


namespace Wme
{
	class Skeleton;

	class WmeDllExport SkeletonManager : public Object
	{
	public:
		SkeletonManager();
		~SkeletonManager();

		bool Initialize();

		Skeleton* GetSkeleton(const WideString& fileName);
		void ReleaseSkeleton(Skeleton*& skeleton);

	private:
		typedef std::map<WideString, Skeleton*> SkeletonMap;
		SkeletonMap m_Skeletons;

	};
}

#endif // __WmeSkeletonManager_H__
