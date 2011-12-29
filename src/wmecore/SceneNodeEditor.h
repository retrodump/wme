// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSceneNodeEditor_H__
#define __WmeSceneNodeEditor_H__


#include "Scene3DEditor.h"
#include "MousePickParams.h"


namespace Wme
{
	class Scene3DBase;
	class Gizmo;
	class MoveGizmo;
	class RotGizmo;
	class ScaleGizmo;
	class NodeSelection;
	class Viewport;
	class ElementCollection;
	class LineElement;

	class WmeDllExport SceneNodeEditor : public Scene3DEditor
	{
	public:
		enum EditorMode
		{
			MODE_NONE,
			MODE_MOVE,
			MODE_ROTATE,
			MODE_SCALE
		};

		SceneNodeEditor(Scene3DBase* stage);
		~SceneNodeEditor();

		RTTI(SceneNodeEditor);

		void Create();
		bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);
		bool HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event);
		void Update();
		void PreRender(Viewport* viewport);

		void OnSceneNodeDestroyed(Ogre::SceneNode* node);

		NodeSelection* GetSelection() { return m_Selection; }
		void UpdateGizmos();

		void SetEditorMode(EditorMode mode);
		EditorMode GetEditorMode() const { return m_Mode; }

		void FocusSelection(Viewport* viewport);

		void SetGizmoLocalMode(bool localMode);
		bool GetGizmoLocalMode() const { return m_GizmoLocalMode; }

		Ogre::Real GetMoveSnap() const { return m_MoveSnap; }
		Ogre::Real GetRotSnap() const { return m_RotSnap; }
		Ogre::Real GetScaleSnap() const { return m_ScaleSnap; }

		bool IsNodeMovable(Ogre::SceneNode* node) const;
		bool IsNodeRotatable(Ogre::SceneNode* node) const;
		bool IsNodeScalable(Ogre::SceneNode* node) const;

		bool IsNodeMeshBased(Ogre::SceneNode* node) const;

	private:
		void CreateGizmos();		
		Ogre::Entity* PickGizmo(Viewport* viewport);
		Ogre::Entity* PickObject(Viewport* viewport);
		void PlaceGizmo();

		Gizmo* GetCurrentGizmo();

		bool PreparePick(Viewport* viewport, MousePickParams& pickParams, Stage*& stage);

		void StartDrag(Viewport* viewport, Ogre::Entity* gizmoEntity);
		void EndDrag();

		MoveGizmo* m_MoveGizmo;
		RotGizmo* m_RotGizmo;
		ScaleGizmo* m_ScaleGizmo;

		NodeSelection* m_Selection;

		Gizmo* m_DragGizmo;
		bool m_IsDragging;
		bool m_DragCancelled;

		EditorMode m_Mode;
		bool m_GizmoLocalMode;


		void DoBoxSelect(Viewport* viewport);
		ElementCollection* m_BoxSelElement;
		LineElement* m_BoxSel;
		bool m_IsBoxSelecting;
		bool m_IsBoxSelectValid;
		int m_BoxSelStartX;
		int m_BoxSelStartY;
		Ogre::Vector4 m_BoxSelPrev;
		Ogre::PlaneBoundedVolumeListSceneQuery* m_VolumeQuery;

		Ogre::Real m_MoveSnap;
		Ogre::Real m_RotSnap;
		Ogre::Real m_ScaleSnap;
	};
}


#endif // __WmeSceneNodeEditor_H__