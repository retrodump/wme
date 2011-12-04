// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "EntityFactory.h"
#include "Entity.h"
#include "EntityGroup.h"
#include "Entity3D.h"
#include "Actor3D.h"
#include "CameraEntity.h"
#include "LightEntity.h"



namespace Wme
{


//////////////////////////////////////////////////////////////////////////
EntityFactory::EntityFactory()
{
	Register("WmeEntityGroup", EntityGroup::CreateInstance);
	Register("WmeEntity3D", Entity3D::CreateInstance);
	Register("WmeActor3D", Actor3D::CreateInstance);
	Register("WmeCamera", CameraEntity::CreateInstance);
	Register("WmeLight", LightEntity::CreateInstance);
}

//////////////////////////////////////////////////////////////////////////
EntityFactory::~EntityFactory()
{
}

//////////////////////////////////////////////////////////////////////////
EntityFactory* EntityFactory::GetInstance()
{
	static EntityFactory s_Instance;
	return &s_Instance;
}

//////////////////////////////////////////////////////////////////////////
Entity* EntityFactory::CreateInstance(const Utf8String& type)
{
	ActivatorMap::iterator it = m_Activators.find(type);
	if (it == m_Activators.end()) return NULL;
	else return (*it).second();
}

//////////////////////////////////////////////////////////////////////////
void EntityFactory::Register(const Utf8String& objectId, Entity::EntityActivator activator)
{
	m_Activators[objectId] = activator;
}


} // namespace Wme
