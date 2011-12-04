// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeNavPath_H__
#define __WmeNavPath_H__


#include "Object.h"


namespace Wme
{
	class WmeDllExport NavPath : public Object
	{
	public:
		NavPath();
		~NavPath();
		
		bool IsReady() const { return m_Ready; }
		void SetReady(bool val) { m_Ready = val; }

		int GetNumPoints() const;
		void AddPoint(const Ogre::Vector3& point);
		const Ogre::Vector3& GetPoint(int index) const;

		void Reserve(size_t count);

		void Reset();

	private:
		PointVector m_Points;
		bool m_Ready;
	};
}

#endif // __WmeNavPath_H__
