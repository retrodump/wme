// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCompositeBBox_H__
#define __WmeCompositeBBox_H__


#include "Object.h"


namespace Wme
{
	class WmeDllExport CompositeBBox : public Object
	{
	public:
		CompositeBBox();
		~CompositeBBox();

		Ogre::Vector3 GetMinimum() const { return m_Minimum; }
		Ogre::Vector3 GetMaximum() const { return m_Maximum; }
		bool IsEmpty() const { return m_Empty; }
		void Clear();

		void AddEntity(const Ogre::Entity* entity);
		void AddNode(const Ogre::SceneNode* node);

		void LocalToWorld(Ogre::Vector3& point) const;

	private:
		void AddPoint(const Ogre::Vector3& point);

		bool m_Empty;
		Ogre::Vector3 m_Minimum;
		Ogre::Vector3 m_Maximum;

		Ogre::Matrix4 m_WorldTransform;
		Ogre::Matrix4 m_InvWorldTransform;
	};
}


#endif // __WmeCompositeBBox_H__