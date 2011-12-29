// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeHelperGizmo_H__
#define __WmeHelperGizmo_H__


#include "GizmoBase.h"


namespace Wme
{
	class HelperEntity3D;
	class Viewport;

	class HelperGizmo : public GizmoBase
	{
	public:
		HelperGizmo(HelperEntity3D* parentHelper);
		~HelperGizmo();

		void Create();
		void Destroy();

		void PreRender(Viewport* viewport);

		Ogre::SceneNode* GetSceneNode() const { return m_SceneNode; }
		HelperEntity3D* GetParentHelper() const { return m_ParentHelper; }

		Ogre::Entity* GetOgreEntity() const { return m_Entity; }

	private:
		HelperEntity3D* m_ParentHelper;

		Ogre::SceneNode* m_SceneNode;
		Ogre::Entity* m_Entity;
	};
}

#endif // __WmeHelperGizmo_H__
