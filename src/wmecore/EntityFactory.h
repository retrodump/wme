// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEntityFactory_H__
#define __WmeEntityFactory_H__


#include "Object.h"
#include "Entity.h"


namespace Wme
{
	class EntityFactory : public Object
	{
	public:		
		~EntityFactory();

		static EntityFactory* GetInstance();

		Entity* CreateInstance(const Utf8String& type);
		void Register(const Utf8String& objectId, Entity::EntityActivator activator);

	private:
		EntityFactory();

		typedef std::map<Utf8String, Entity::EntityActivator> ActivatorMap;
		ActivatorMap m_Activators;
	};
}

#endif // __WmeEntityFactory_H__
