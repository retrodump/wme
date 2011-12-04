// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeNavMeshRenderer_H__
#define __WmeNavMeshRenderer_H__


#include "Object.h"
#include "NavMesh.h"


namespace Wme
{
	class NavMesh;
	class NavNode;
	class Stage;
	class Viewport;
	class LineElement3D;
	class TriangleElement3D;
	class NavMeshEditor;


	class WmeDllExport NavMeshRenderer : public Object
	{
	public:
		NavMeshRenderer(NavMeshEditor* editor);
		~NavMeshRenderer();

		typedef std::set<NavNode*> NodeCollection;

		void Create();
		void UpdateGeometry();

		void SetPath(const NavMesh::PathPointList& path);

	private:
		void PathTest();

		NavMeshEditor* m_Editor;

		Ogre::SceneNode* m_SceneNode;
		LineElement3D* m_LineRenderer;
		TriangleElement3D* m_TriRenderer;
		LineElement3D* m_PathRenderer;
		static const int NAVMESH_RENDER_QUEUE_ID = Ogre::RENDER_QUEUE_SKIES_LATE + 1;

		Ogre::ColourValue m_ColPath;

		Ogre::ColourValue m_ColNormalNode;
		Ogre::ColourValue m_ColBlockedNode;
		Ogre::ColourValue m_ColSelectedNode;
		Ogre::ColourValue m_ColHoverNode;
		Ogre::ColourValue m_ColHoverSelectedNode;

		Ogre::ColourValue m_ColNormalEdge;
		Ogre::ColourValue m_ColOuterEdge;
		Ogre::ColourValue m_ColBlockedEdge;
		Ogre::ColourValue m_ColSelectedEdge;
		Ogre::ColourValue m_ColHoverEdge;
		Ogre::ColourValue m_ColHoverSelectedEdge;
	};
}

#endif // __WmeNavMeshRenderer_H__