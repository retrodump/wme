// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeNodeSelection_H__
#define __WmeNodeSelection_H__


#include "Object.h"


namespace Wme
{
	class CompositeBBox;
	class SceneNodeEditor;

	class WmeDllExport NodeSelection : public Object
	{
	public:

		class WmeDllExport Listener 
		{
		public:
			Listener() {}
			virtual ~Listener() {}

			virtual void NodeSelectionChanged() {};
			virtual void NodeSelectionPositionChanged() {};
			virtual void NodeSelectionOrientationChanged() {};
			virtual void NodeSelectionScaleChanged() {};
		};


		NodeSelection(SceneNodeEditor* editor);
		~NodeSelection();

		typedef std::list<Ogre::SceneNode*> NodeList;
		NodeList& GetNodes() { return m_Nodes; }

		void Clear(bool emitEvent = true);
		void AddNode(Ogre::SceneNode* node);
		void SetNode(Ogre::SceneNode* node);
		void RemoveNode(Ogre::SceneNode* node);
		bool IsSelected(Ogre::SceneNode* node);

		void Refresh();

		Ogre::SceneNode* GetAnchorNode();

		const Ogre::Vector3& GetPosition();
		const Ogre::Quaternion& GetOrientation();
		const Ogre::Vector3& GetScale();

		void SetOrientation(const Ogre::Quaternion& q, bool local, bool emitEvent = true);
		void ScaleBy(const Ogre::Vector3& scale, bool emitEvent = true);
		void MoveBy(const Ogre::Vector3& moveDiff, bool local, bool emitEvent = true);

		bool IsEmpty() const;
		bool IsMultiSelect() const;

		const Ogre::Vector3& GetOrigin();
		const Ogre::Vector3& GetScaleOrigin();

		const CompositeBBox* GetBBox() const { return m_BBox; }

		void StartBulkSelection();
		void EndBulkSelection();

		SceneNodeEditor* GetEditor() const { return m_Editor; }

		void AddListener(Listener* listener);
		void RemoveListener(Listener* listener);

		Ogre::Vector3 GetCenter();
		Ogre::Vector3 GetHalfSize();

		bool IsMovable() const;
		bool IsRotatable() const;
		bool IsScalable() const;

		static void HighlightNode(Ogre::SceneNode* node, bool highlight);
		static void HighlightEntity(Ogre::Entity* entity, bool highlight);

	private:
		SceneNodeEditor* m_Editor;

		bool m_IsBulkSelecting;

		NodeList m_Nodes;
		CompositeBBox* m_BBox;

		Ogre::Vector3 m_Origin;
		
		void RebuildBBox();

		void EmitSelectionChanged();
		void EmitPositionChanged();
		void EmitOrientationChanged();
		void EmitScaleChanged();

		typedef std::vector<Listener*> ListenerList;
		ListenerList m_Listeners;

	};
}

#endif // __WmeNodeSelection_H__