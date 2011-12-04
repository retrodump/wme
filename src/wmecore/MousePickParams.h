// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMousePickParams_H__
#define __WmeMousePickParams_H__


#include "Object.h"


namespace Wme
{
	class InteractiveObject;
	class Viewport;

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport MousePickParams
	{
	public:
		MousePickParams();
		~MousePickParams();

		Ogre::Vector3 Point;
		Ogre::Vector3 Normal;
		int RenderQueueId;
		bool QueryUV;
		
		Ogre::Vector3 ResultPoint;
		Ogre::Vector2 ResultUV;
		Ogre::Entity* ResultEntity;
		Ogre::SubEntity* ResultSubEntity;
	};


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport MousePickResult
	{
	public:
		MousePickResult();
		~MousePickResult();

		InteractiveObject* Object;
		Ogre::Vector3 HitPoint;
		Ogre::Vector2 HitUV;
		Wme::Viewport* Viewport;
	};


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport PickableObject
	{
	public:
		enum ObjectType
		{
			PICKABLE_GIZMO,
			PICKABLE_ENTITY,
			PICKABLE_RTT_VIEW
		};

		PickableObject(ObjectType type, Object* object)
		{
			m_Type = type;
			m_Object = object;
		}

		// required by Ogre::Any, d-oh!
		friend std::ostream& operator<<(std::ostream& os, const PickableObject& obj)
		{
			os << obj.GetType();
			os << obj.GetObject();
			return os;
		}

		ObjectType GetType() const { return m_Type; }
		Object* GetObject() const { return m_Object; }

	private:
		ObjectType m_Type;
		Object* m_Object;
	};
}

#endif // __WmeMousePickParams_H__
