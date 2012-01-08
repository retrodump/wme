// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeElement2D_H__
#define __WmeElement2D_H__


namespace Wme
{
	class SceneNode2D;

	class WmeDllExport Element2D
	{
	public:
		Element2D();
		virtual ~Element2D();

		SceneNode2D* GetParentNode() const { return m_ParentNode; }
		
		virtual void OnAttached(SceneNode2D* sceneNode);
		virtual void OnDetached(SceneNode2D* sceneNode);
		void DetachFromParent();

		bool IsAttached() const;

		virtual void AddGeometry() {};

	protected:
		SceneNode2D* m_ParentNode;
	};
}

#endif // __WmeElement2D_H__