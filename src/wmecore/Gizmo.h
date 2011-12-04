// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGizmo_H__
#define __WmeGizmo_H__


#include "GizmoBase.h"


namespace Wme
{
	class NodeSelection;
	class Camera;
	class Viewport;
	class SceneNodeEditor;

	class WmeDllExport Gizmo : public GizmoBase
	{
	public:
		Gizmo(SceneNodeEditor* editor);
		virtual ~Gizmo();

		virtual void Create() = 0;
		
		virtual void ConformTo(NodeSelection* selection);
		
		virtual void HighlightPart(Ogre::Entity* entity = NULL) = 0;
		virtual void HighlightDragPart() = 0;

		virtual bool GetAxis(Ogre::Entity* part, Ogre::Vector3& axis) = 0;
		virtual bool GetNormal(Camera* camera, Ogre::Entity* part, Ogre::Vector3& normal) = 0;

		virtual bool StartDrag(Viewport* viewport, Ogre::Entity* part) = 0;
		virtual void EndDrag() = 0;
		virtual void UpdateDrag() = 0;
		virtual void Reset() = 0;

		virtual void SetVisible(bool visible, NodeSelection* selection) { m_IsVisible = visible; }
		bool GetVisible() const { return m_IsVisible; }

		virtual void PreRender(Viewport* viewport, NodeSelection* selection);


		Ogre::SceneNode* GetSceneNode() const { return m_SceneNode; }
		SceneNodeEditor* GetEditor() const { return m_Editor; }

		static const int GIZMO_RENDER_QUEUE_ID = Ogre::RENDER_QUEUE_SKIES_LATE + 1;

	protected:
		void DestroyObject(Ogre::SceneNode* node, Ogre::Entity* entity);

		SceneNodeEditor* m_Editor;

		Ogre::SceneNode* m_SceneNode;
		Ogre::Entity* m_Entity;
		
		bool m_IsVisible;
	};
}

#endif // __WmeGizmo_H__