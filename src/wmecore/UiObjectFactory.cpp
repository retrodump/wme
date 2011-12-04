// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "UiObjectFactory.h"
#include "UiObject.h"
#include "UiWindow.h"
#include "UiButton.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
UiObjectFactory::UiObjectFactory()
{
	Register("WmeWindow", UiWindow::CreateInstance);
	Register("WmeButton", UiButton::CreateInstance);
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
UiObject* UiObjectFactory::CreateInstance(GuiStage* parentStage, const Utf8String& type)
{
	ActivatorMap::iterator it = m_Activators.find(type);
	if (it == m_Activators.end()) return NULL;
	else return (*it).second(parentStage);
}

//////////////////////////////////////////////////////////////////////////
void UiObjectFactory::Register(const Utf8String& objectId, UiObject::UiObjectActivator activator)
{
	m_Activators[objectId] = activator;
}


} // namespace Wme
