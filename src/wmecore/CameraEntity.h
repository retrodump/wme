// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCameraEntity_H__
#define __WmeCameraEntity_H__


#include "Entity3DBase.h"


namespace Wme
{
	class Camera;
	class CameraGizmo;

	class WmeDllExport CameraEntity : public Entity3DBase
	{
	public:
		CameraEntity();
		virtual ~CameraEntity();

		static Entity* CreateInstance() { return new CameraEntity(); }
		virtual WideString GetEntitySubtype() const { return L"Camera"; }

		bool IsGizmoVisible() const { return m_IsGizmoVisible; }
		void SetGizmoVisible(bool visible);

		void PutToStage(Scene3DBase* stage, Entity3DBase* parentEntity = NULL);
		void RemoveFromStage();

		void PreRender(Viewport* viewport);

		Camera* GetCamera() const { return m_Camera; }
		Ogre::Camera* GetOgreCamera() const;

		// editor support
		virtual Ogre::Entity* GetSelectableEntity() const;

		enum ProjectionType
		{
			PROJECTION_ORTHOGRAPHIC,
			PROJECTION_PERSPECTIVE
		};

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type);

		void SetFOVy(const Ogre::Radian& fovy);
		const Ogre::Radian& GetFOVy() const { return m_FOVy; }

		void SetNearClipDistance(Ogre::Real distance);
		Ogre::Real GetNearClipDistance() const { return m_NearClipDistance; }

		void SetFarClipDistance(Ogre::Real distance);
		Ogre::Real GetFarClipDistance() const { return m_FarClipDistance; }

		void SetAspectRatio(Ogre::Real ratio);
		Ogre::Real GetAspectRatio() const { return m_AspectRatio; }

		void SetCustomAspectRatio(bool customRatio);
		bool HasCustomAspectRatio() const { return m_CustomAspectRatio; }

		void SetOrthoWindowHeight(Ogre::Real height);
		Ogre::Real GetOrthoWindowHeight() const { return m_OrthoWindowHeight; }


		// ScriptableObject
		RTTI(Camera);
		virtual WideString ConvertToString() { return L"Camera object"; }
		virtual WideString GetTypeName() const { return L"Camera"; }

		// script interface
		virtual void RegisterScriptInterface();

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);
		virtual Utf8String GetDocRootName() { return "WmeCamera"; }

	private:
		Camera* m_Camera;

		bool m_IsGizmoVisible;
		CameraGizmo* m_Gizmo;

		void RebuildOgreCamera();

		ProjectionType m_ProjectionType;
		Ogre::Radian m_FOVy;
		Ogre::Real m_FarClipDistance;
		Ogre::Real m_NearClipDistance;
		Ogre::Real m_AspectRatio;
		bool m_CustomAspectRatio;
		Ogre::Real m_OrthoWindowHeight;
	};
}

#endif // __WmeCameraEntity_H__
