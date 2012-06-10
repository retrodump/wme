// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiObjectFactory.h"
#include "UiObjectOld.h"
#include "UiWindowOld.h"
#include "UiButtonOld.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiObjectFactory::UiObjectFactory()
{
	Register("WmeWindow", UiWindowOld::CreateInstance);
	Register("WmeButton", UiButtonOld::CreateInstance);
}

//////////////////////////////////////////////////////////////////////////
UiObjectFactory::~UiObjectFactory()
{
}

//////////////////////////////////////////////////////////////////////////
UiObjectFactory* UiObjectFactory::GetInstance()
{
	static UiObjectFactory s_Instance;
	return &s_Instance;
}

//////////////////////////////////////////////////////////////////////////
UiObjectOld* UiObjectFactory::CreateInstance(GuiStage* parentStage, const Utf8String& type)
{
	ActivatorMap::iterator it = m_Activators.find(type);
	if (it == m_Activators.end()) return NULL;
	else return (*it).second(parentStage);
}

//////////////////////////////////////////////////////////////////////////
void UiObjectFactory::Register(const Utf8String& objectId, UiObjectOld::UiObjectActivator activator)
{
	m_Activators[objectId] = activator;
}


} // namespace Wme
