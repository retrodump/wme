// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeLightGizmo_H__
#define __WmeLightGizmo_H__


#include "GizmoBase.h"


namespace Wme
{
	class LightEntity;
	class Viewport;

	class LightGizmo : public GizmoBase
	{
	public:
		LightGizmo(LightEntity* parentLight);
		virtual ~LightGizmo();

		void Create();
		void Destroy();

		void PreRender(Viewport* viewport);

		Ogre::SceneNode* GetSceneNode() const { return m_SceneNode; }
		LightEntity* GetParentLight() const { return m_ParentLight; }

		Ogre::Entity* GetOgreEntity() const { return m_Entity; }

	private:
		LightEntity* m_ParentLight;

		Ogre::SceneNode* m_SceneNode;
		Ogre::Entity* m_Entity;

	};
}

#endif // __WmeLightGizmo_H__
