// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeHelperEntity3D_H__
#define __WmeHelperEntity3D_H__


#include "Entity3DBase.h"


namespace Wme
{
	class HelperGizmo;

	class HelperEntity3D : public Entity3DBase
	{
	public:
		HelperEntity3D();
		~HelperEntity3D();

		static Entity* CreateInstance() { return new HelperEntity3D(); }
		virtual WideString GetEntitySubtype() const { return L"Helper3D"; }

		bool IsGizmoVisible() const { return m_IsGizmoVisible; }
		void SetGizmoVisible(bool visible);

		void PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity = NULL);

		void PreRender(Viewport* viewport);

		virtual void SetActive(bool val);

		// editor support
		virtual Ogre::Entity* GetSelectableEntity() const;

		// ScriptableObject
		RTTI(HelperEntity3D);
		virtual WideString ConvertToString() { return L"Helper object"; }
		virtual WideString GetTypeName() const { return L"Helper3D"; }

		// script interface
		virtual void RegisterScriptInterface();

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeHelper3D"; }

	private:
		bool m_IsGizmoVisible;
		HelperGizmo* m_Gizmo;

	};
}


#endif // __WmeHelperEntity3D_H__
