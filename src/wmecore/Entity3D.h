// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeEntity3D_H__
#define __WmeEntity3D_H__


#include "MeshEntity.h"


namespace Wme
{
	class Entity3D : public MeshEntity
	{
	public:		
		Entity3D();
		virtual ~Entity3D();

		static Entity* CreateInstance() { return new Entity3D(); }
		virtual WideString GetEntitySubtype() const { return L"Entity3D"; }

		// ScriptableObject
		RTTI(Entity3D);
		virtual WideString ConvertToString() { return L"Entity3D object"; }
		virtual WideString GetTypeName() const { return L"Entity3D"; }

		// script interface
		virtual void RegisterScriptInterface();

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeEntity3D"; }

	};
}

#endif // __WmeEntity3D_H__

