// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeNavMeshEditor_H__
#define __WmeNavMeshEditor_H__


#include "Scene3DEditor.h"
#include "NavMesh.h"


namespace Wme
{
	class Scene3DBase;
	class NavMesh;
	class NavNode;
	class NavEdge;
	class NavMeshRenderer;

	class NavMeshEditor : public Scene3DEditor, public NavMesh::Listener
	{
	public:
		NavMeshEditor(Scene3DBase* stage);
		~NavMeshEditor();

		void Create();
		bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);
		bool HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event);
		void Update();
		void PreRender(Viewport* viewport);

		void SetNavMesh(NavMesh* navMesh);
		NavMesh* GetNavMesh() { return m_NavMesh; }

		NavNode* GetHoverNode();
		NavEdge* GetHoverEdge();

		void SetSelectedNode(NavNode* node);
		void AddSelectedNode(NavNode* node);
		void RemoveSelectedNode(NavNode* node);
		void ClearSelectedNodes();
		bool IsNodeSelected(NavNode* node) const;

		void SetSelectedEdge(NavEdge* edge);
		void AddSelectedEdge(NavEdge* edge);
		void RemoveSelectedEdge(NavEdge* edge);
		void ClearSelectedEdges();
		bool IsEdgeSelected(NavEdge* edge) const;

		// NavMesh::Listener
		void OnPathFound(const NavMesh::PathPointList& path);
		void OnDestroy();

	private:
		enum EditorMode
		{
			MODE_NODES,
			MODE_EDGES
		};

		EditorMode m_Mode;


		void SetHover(Viewport* viewport);

		NavMesh* m_NavMesh;
		NavMeshRenderer* m_NavMeshRenderer;

		typedef std::set<NavNode*> NodeCollection;
		NodeCollection m_SelectedNodes;

		typedef std::set<NavEdge*> EdgeCollection;
		EdgeCollection m_SelectedEdges;

		NavNode* m_HoverNode;
		NavEdge* m_HoverEdge;

		bool m_IsDragging;
		bool m_DragSelecting;

	};
}

#endif // __WmeNavMeshEditor_H__