// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEntityGroup_H__
#define __WmeEntityGroup_H__


#include "Entity3DBase.h"


namespace Wme
{
	class WmeDllExport EntityGroup : public Entity3DBase
	{
	public:
		EntityGroup();
		~EntityGroup();

		static Entity* CreateInstance() { return new EntityGroup(); }

        // ScriptableObject
		RTTI(EntityGroup);
		virtual WideString ConvertToString() { return L"EntityGroup object"; }
		virtual WideString GetTypeName() const { return L"EntityGroup"; }

		// script interface
		virtual void RegisterScriptInterface();

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeEntityGroup"; }

	};
}

#endif // __WmeEntityGroup_H__
