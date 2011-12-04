// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeLightEntity_H__
#define __WmeLightEntity_H__


#include "Entity3DBase.h"


namespace Wme
{
	class LightGizmo;

	class WmeDllExport LightEntity : public Entity3DBase
	{
	public:
		LightEntity();
		virtual ~LightEntity();

		static Entity* CreateInstance() { return new LightEntity(); }
		virtual WideString GetEntitySubtype() const { return L"Light"; }

		bool IsGizmoVisible() const { return m_IsGizmoVisible; }
		void SetGizmoVisible(bool visible);

		void PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity = NULL);
		void RemoveFromStage();

		void PreRender(Viewport* viewport);

		Ogre::Light* GetOgreLight() const { return m_Light; }

		// editor support
		virtual Ogre::Entity* GetSelectableEntity() const;


		enum LightType
		{
			LIGHT_POINT,
			LIGHT_DIRECTIONAL,
			LIGHT_SPOT
		};

		LightType GetLightType() const { return m_LightType; }
		void SetLightType(LightType type);

		const Ogre::ColourValue GetDiffuseColor() const { return m_DiffuseColor; }
		void SetDiffuseColor(const Ogre::ColourValue& color);

		const Ogre::ColourValue GetSpecularColor() const { return m_SpecularColor; }
		void SetSpecularColor(const Ogre::ColourValue& color);

		void SetAttenuation(Ogre::Real range, Ogre::Real constant, Ogre::Real linear, Ogre::Real quadratic);
		Ogre::Real GetAttenuationRange() const { return m_Range; }
		Ogre::Real GetAttenuationConstant() const { return m_AttenuationConstant; }
		Ogre::Real GetAttenuationLinear() const { return m_AttenuationLinear; }
		Ogre::Real GetAttenuationQuadratic() const { return m_AttenuationQuadratic; }

		void SetSpotlightRange(const Ogre::Radian& innerAngle, const Ogre::Radian& outerAngle, Ogre::Real falloff = 1.0);

		const Ogre::Radian& GetSpotlightInnerAngle() const { return m_SpotlightInnerAngle; }
		void SetSpotlightInnerAngle(const Ogre::Radian& val);

		const Ogre::Radian& GetSpotlightOuterAngle() const { return m_SpotlightOuterAngle; }
		void SetSpotlightOuterAngle(const Ogre::Radian& val);

		Ogre::Real GetSpotlightFalloff() const { return m_SpotlightFalloff; }
		void SetSpotlightFalloff(Ogre::Real val);

		bool GetCastShadows() const { return m_CastShadows; }
		void SetCastShadows(bool val);

		virtual void SetActive(bool val);


		// ScriptableObject
		RTTI(LightEntity);
		virtual WideString ConvertToString() { return L"Light object"; }
		virtual WideString GetTypeName() const { return L"Light"; }

		// script interface
		virtual void RegisterScriptInterface();

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeLight"; }

	private:
		Ogre::Light* m_Light;

		bool m_IsGizmoVisible;
		LightGizmo* m_Gizmo;

		void RebuildOgreLight();


		LightType m_LightType;
		Ogre::ColourValue m_DiffuseColor;
		Ogre::ColourValue m_SpecularColor;

		Ogre::Real m_Range;
		Ogre::Real m_AttenuationConstant;
		Ogre::Real m_AttenuationLinear;
		Ogre::Real m_AttenuationQuadratic;

		Ogre::Radian m_SpotlightOuterAngle;
		Ogre::Radian m_SpotlightInnerAngle;
		Ogre::Real m_SpotlightFalloff;

		bool m_CastShadows;
	};
}

#endif // __WmeLightEntity_H__
