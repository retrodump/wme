// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedDocScene_H__
#define __ArmedDocScene_H__


#include "DocumentView.h"
#include "NodeSelection.h"
#include "SceneNodeEditor.h"


using namespace Wme;

namespace Wme
{
	class Scene3DBase;
}


namespace Armed
{
	class WmeWidget;
	class SceneNodeNavigator;
	class SceneNodeModel;
	class PropTransform;
	class Scene3DBar;
	
	class DocScene : public DocumentView, public NodeSelection::Listener
	{
		Q_OBJECT

	public:
		DocScene(QWidget* parent);
		~DocScene();

		bool LoadData(const QString& fileName);
		bool SaveData(const QString& fileName);
		bool NewData();

		QString GetDocExtension() const { return tr("scene"); }
		QString GetDocDescription() const { return tr("Scene files"); }
		virtual QString GetDefaultName() const { return tr("Scene"); }

		void OnActivate();
		void BuildActions();

		void SetScene(Scene3DBase* scene);
		Scene3DBase* GetScene() const { return m_Scene; }

		SceneNodeModel* GetSceneNodeModel() const { return m_SceneNodeModel; }

		void SelectNodes(const QList<Ogre::SceneNode*>& nodes);

		// NodeSelection::Listener
		void NodeSelectionChanged();
		void NodeSelectionPositionChanged();
		void NodeSelectionOrientationChanged();
		void NodeSelectionScaleChanged();

	signals:
		void SceneChanged();

	private slots:
		void OnMeshOrSkeletonChanged();
		void OnTransfromChanged();

	private:
		WmeWidget* m_View;
		Scene3DBar* m_SceneBar;
		SceneNodeNavigator* m_NodeNav;

		Scene3DBase* m_Scene;
		SceneNodeEditor* m_Editor;
		
		SceneNodeModel* m_SceneNodeModel;		

		void CreateSceneNodeEditor();
		PropTransform* m_PropTransform;

		typedef std::map<QAction*, SceneNodeEditor::EditorMode> TransformActionsMap;
		TransformActionsMap m_TransformActionsMap;


		void UpdateActions();
	
	};
}

#endif // __ArmedDocScene_H__
