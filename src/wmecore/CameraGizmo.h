// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCameraGizmo_H__
#define __WmeCameraGizmo_H__


#include "GizmoBase.h"


namespace Wme
{
	class CameraEntity;
	class Viewport;

	class CameraGizmo : public GizmoBase
	{
	public:
		CameraGizmo(CameraEntity* parentCamera);
		virtual ~CameraGizmo();

		void Create();
		void Destroy();

		void PreRender(Viewport* viewport);

		Ogre::SceneNode* GetSceneNode() const { return m_SceneNode; }
		CameraEntity* GetParentCamera() const { return m_ParentCamera; }

		Ogre::Entity* GetOgreEntity() const { return m_Entity; }

	private:
		CameraEntity* m_ParentCamera;

		Ogre::SceneNode* m_SceneNode;
		Ogre::Entity* m_Entity;
	};
}

#endif // __WmeCameraGizmo_H__
