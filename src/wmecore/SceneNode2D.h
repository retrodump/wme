// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSceneNode2D_H__
#define __WmeSceneNode2D_H__


#include "Transform2D.h"


namespace Wme
{
	class Element2D;

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

		const Ogre::Vector2& GetPosition() const { return m_Position; }
		void SetPosition(const Ogre::Vector2& pos);
		void SetPosition(float x, float y);

		const Ogre::Degree& GetRotation() const { return m_Rotation; }
		void SetRotation(const Ogre::Degree& angle);
		void SetRotation(float degrees);

		const Ogre::Vector2& GetScale() const { return m_Scale; }
		void SetScale(const Ogre::Vector2& scale);
		void SetScale(float x, float y);

		const Ogre::Vector2& GetDerivedPosition() const { return m_DerivedPosition; }
		void SetDerivedPosition(const Ogre::Vector2& pos);
		void SetDerivedPosition(float x, float y);

		const Ogre::Degree& GetDerivedRotation() const { return m_DerivedRotation; }
		void SetDerivedRotation(const Ogre::Degree& angle);
		void SetDerivedRotation(float degrees);

		const Ogre::Vector2& GetDerivedScale() const { return m_DerivedScale; }
		void SetDerivedScale(const Ogre::Vector2& scale);
		void SetDerivedScale(float x, float y);

		const Transform2D& GetSceneTransform() const { return m_SceneTransform; }

		void AttachElement(Element2D* element);
		void DetachElement();
		Element2D* GetAttachedElement() const { return m_AttachedElement; }

		void UpdateTransform(bool forceUpdate = false, bool updateChildren = true);

		Ogre::Vector2 PositionSceneToLocal(const Ogre::Vector2& pos) const;
		Ogre::Degree RotationSceneToLocal(const Ogre::Degree& angle) const;
		Ogre::Vector2 ScaleSceneToLocal(const Ogre::Vector2& scale) const;

	private:
		mutable NodeList m_Children;
		SceneNode2D* m_ParentNode;
		
		int m_ZOrder;
		Ogre::Vector2 m_Position;
		Ogre::Degree m_Rotation;
		Ogre::Vector2 m_Scale;

		Ogre::Vector2 m_DerivedPosition;
		Ogre::Degree m_DerivedRotation;
		Ogre::Vector2 m_DerivedScale;

		Transform2D m_SceneTransform;

		mutable bool m_SortNeeded;
		void SortChildren() const;
		static bool ZOrderComparer(SceneNode2D* node1, SceneNode2D* node2);

		bool m_TransformDirty;
		bool m_GeometryDirty;

		void SetTransformDirty();
		void SetGeometryDirty();

		void UpdateTransfromInternal();

		Element2D* m_AttachedElement;
	};
}

#endif // __WmeSceneNode2D_H__
