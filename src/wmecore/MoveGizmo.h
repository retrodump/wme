// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMoveGizmo_H__
#define __WmeMoveGizmo_H__


#include "Gizmo.h"


namespace Wme
{
	class Viewport;

	class WmeDllExport MoveGizmo : public Gizmo
	{
	public:
		MoveGizmo(SceneNodeEditor* editor);
		~MoveGizmo();

		void Create();

		void HighlightPart(Ogre::Entity* entity = NULL);
		void HighlightDragPart();

		bool GetAxis(Ogre::Entity* part, Ogre::Vector3& axis);
		bool GetNormal(Camera* camera, Ogre::Entity* part, Ogre::Vector3& normal);

		bool StartDrag(Viewport* viewport, Ogre::Entity* part);
		void EndDrag();
		void UpdateDrag();
		void Reset();

		void SetVisible(bool visible, NodeSelection* selection);

	private:
		void CreateArrow(Ogre::SceneNode* parentNode, const Ogre::MeshPtr mesh, const WideString& name, Ogre::SceneNode*& node, Ogre::Entity*& entity);
		
		Ogre::SceneNode* m_NodeX;
		Ogre::SceneNode* m_NodeY;
		Ogre::SceneNode* m_NodeZ;

		Ogre::Entity* m_EntityX;
		Ogre::Entity* m_EntityY;
		Ogre::Entity* m_EntityZ;

		Ogre::Entity* m_DragPart;
		Ogre::Plane m_DragPlane;
		Viewport* m_DragViewport;
		Ogre::Vector3 m_DragOrigin;
		Ogre::Vector3 m_DragStoredPos;
	};
}

#endif // __WmeMoveGizmo_H__