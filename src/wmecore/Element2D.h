// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeElement2D_H__
#define __WmeElement2D_H__


namespace Wme
{
	class SceneNode2D;
	class Element2D;
	class InteractiveObject;

	typedef std::list<Element2D*> Element2DList;

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

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

		InteractiveObject* GetOwner() const { return m_Owner; }
		void SetOwner(InteractiveObject* val) { m_Owner = val; }

		void SetDirty();

		virtual bool IsTransparentAt(float x, float y) { return false; }

		virtual void AddGeometry() {};

	protected:
		SceneNode2D* m_ParentNode;
		bool m_IsVisible;

		InteractiveObject* m_Owner;
	};
}

#endif // __WmeElement2D_H__