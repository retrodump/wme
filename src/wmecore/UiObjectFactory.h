// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiObjectFactory_H__
#define __WmeUiObjectFactory_H__


#include "Object.h"
#include "UiObjectOld.h"


namespace Wme
{
	class GuiStage;

	class UiObjectFactory : public Object
	{
	public:		
		~UiObjectFactory();

		static UiObjectFactory* GetInstance();

		UiObjectOld* CreateInstance(GuiStage* parentStage, const Utf8String& type);
		void Register(const Utf8String& objectId, UiObjectOld::UiObjectActivator activator);

	private:
		UiObjectFactory();

		typedef std::map<Utf8String, UiObjectOld::UiObjectActivator> ActivatorMap;
		ActivatorMap m_Activators;
	};
}

#endif // __WmeUiObjectFactory_H__
