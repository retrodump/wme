// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSceneNode2D_H__
#define __WmeSceneNode2D_H__

namespace Wme
{
	class WmeDllExport SceneNode2D
	{
	public:
		SceneNode2D();
		virtual ~SceneNode2D();

		typedef std::list<SceneNode2D*> NodeList;

		void AddChild(SceneNode2D* child, int zOrder = 0);
		void RemoveChild(SceneNode2D* child);

		SceneNode2D* GetParentNode() const { return m_ParentNode; }
		void SetParentNode(SceneNode2D* newParent);

		size_t GetNumChildren() const { return m_Children.size(); }
		SceneNode2D* GetChild(size_t index) const;

		int GetZOrder() const { return m_ZOrder; }
		void SetZOrder(int zOrder);

	private:
		mutable NodeList m_Children;
		SceneNode2D* m_ParentNode;
		
		int m_ZOrder;


		mutable bool m_SortNeeded;
		void SortChildren() const;
		static bool ZOrderComparer(SceneNode2D* node1, SceneNode2D* node2);
	};
}

#endif // __WmeSceneNode2D_H__
