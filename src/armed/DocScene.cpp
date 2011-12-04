// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "DocScene.h"
#include "MainWindow.h"
#include "Navigator.h"
#include "WmeWidget.h"
#include "SceneNodeNavigator.h"
#include "Scene3DBase.h"
#include "Entity3DBase.h"
#include "MeshEntity.h"
#include "CameraEntity.h"
#include "LightEntity.h"
#include "SceneNodeModel.h"
#include "SceneNodeEditor.h"
#include "PropWindow.h"
#include "PropScene3D.h"
#include "PropSkybox.h"
#include "PropTransform.h"
#include "PropMaterial.h"
#include "PropMesh.h"
#include "PropCamera.h"
#include "PropLight.h"

// temp
#include "Game.h"
#include "ContentManager.h"
#include "FullScene.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
DocScene::DocScene(QWidget* parent) : DocumentView(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);

	m_View = new WmeWidget(this);
	m_View->CreateWmeView();
	m_View->AddIgnoredKeySequence(QKeySequence::NextChild);
	m_View->AddIgnoredKeySequence(QKeySequence::PreviousChild);
	layout->addWidget(m_View);

	m_Scene = NULL;
	m_Editor = NULL;
	m_PropTransform = NULL;

	m_SceneNodeModel = new SceneNodeModel(this);		
	m_NodeNav = new SceneNodeNavigator(this, this);

	SetScene(Game::GetInstance()->GetContentMgr()->GetTestScene());
}

//////////////////////////////////////////////////////////////////////////
DocScene::~DocScene()
{
	if (m_Editor)
	{
		m_Editor->GetSelection()->RemoveListener(this);
	}
}

//////////////////////////////////////////////////////////////////////////
void DocScene::OnActivate()
{
	MainWindow::GetInstance()->GetNavigator()->SetNavigator(m_NodeNav);
	NodeSelectionChanged();
	m_View->setFocus(Qt::OtherFocusReason);
}

//////////////////////////////////////////////////////////////////////////
bool DocScene::LoadData(const QString& fileName)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool DocScene::SaveData(const QString& fileName)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool DocScene::NewData()
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
void DocScene::SetScene(Scene3DBase* scene)
{
	m_Scene = scene;
	m_Editor = NULL;
	
	// populate scene structure
	if (m_Scene)
	{
		// tell the engine to create editor
		CreateSceneNodeEditor();

		// update model
		m_SceneNodeModel->SetScene(this);
		m_NodeNav->FitColumns();
	
		emit SceneChanged();	
	}
}

//////////////////////////////////////////////////////////////////////////
void DocScene::CreateSceneNodeEditor()
{
	m_Editor = new SceneNodeEditor(m_Scene);
	m_Editor->Create();
	m_Editor->GetSelection()->AddListener(this);

	m_Scene->SetEditor(m_Editor);
}

//////////////////////////////////////////////////////////////////////////
void DocScene::NodeSelectionChanged()
{
	QModelIndexList indexList;

	NodeSelection::NodeList list = m_Editor->GetSelection()->GetNodes();
	foreach (Ogre::SceneNode* node, list)
	{		
		QModelIndex index = m_SceneNodeModel->IndexFromNode(node);
		if (index.isValid()) indexList.append(index);
	}
	m_NodeNav->SelectIndexRange(indexList);

	PropWindow* propWindow = MainWindow::GetInstance()->GetPropWindow();

	// clear current properties
	propWindow->ClearProperties();
	m_PropTransform = NULL;

	// build the new properties
	if (!m_Editor->GetSelection()->IsEmpty())
	{
		Ogre::SceneNode* anchorNode = m_Editor->GetSelection()->GetAnchorNode();
		if (anchorNode == m_Scene->GetRootNode())
		{
			// scene properties
			PropScene3D* propScene = new PropScene3D();
			propScene->SetScene(m_Scene);
			propWindow->AddProperty(tr("Scene"), propScene);

			// skybox
			PropSkybox* propSkybox = new PropSkybox();
			propSkybox->SetScene(m_Scene);
			propWindow->AddProperty(tr("Skybox"), propSkybox);
		}
		else
		{
			// transform
			m_PropTransform = new PropTransform();
			m_PropTransform->SetSelection(m_Editor->GetSelection());

			propWindow->AddProperty(tr("Placement"), m_PropTransform);
		
			if (anchorNode && m_Editor->IsNodeMeshBased(anchorNode))
			{
				// mesh based entity
				Entity3DBase* entity = Entity3DBase::NodeToEntity(anchorNode);
				if (entity)
				{
					// mesh
					PropMesh* propMesh = new PropMesh();
					propMesh->SetEntity(static_cast<MeshEntity*>(entity));
					propWindow->AddProperty(tr("Mesh"), propMesh);
					connect(propMesh, SIGNAL(MeshChanged()), this, SLOT(OnMeshOrSkeletonChanged()));
					connect(propMesh, SIGNAL(SkeletonChanged()), this, SLOT(OnMeshOrSkeletonChanged()));

					// material
					PropMaterial* propMaterial = new PropMaterial();
					propMaterial->SetEntity(static_cast<MeshEntity*>(entity));
					propWindow->AddProperty(tr("Material"), propMaterial);
				}
			}

			// specific entities
			Entity3DBase* entity = Entity3DBase::NodeToEntity(anchorNode);
			if (entity)
			{
				if (entity->GetEntitySubtype() == L"Camera")
				{
					PropCamera* propCamera = new PropCamera();
					propCamera->SetCamera(static_cast<CameraEntity*>(entity));
					propWindow->AddProperty(tr("Camera"), propCamera);
				}
				else if (entity->GetEntitySubtype() == L"Light")
				{
					PropLight* propLight = new PropLight();
					propLight->SetLight(static_cast<LightEntity*>(entity));
					propWindow->AddProperty(tr("Light"), propLight);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void DocScene::NodeSelectionPositionChanged()
{
	if (m_PropTransform) m_PropTransform->DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void DocScene::NodeSelectionOrientationChanged()
{
	if (m_PropTransform) m_PropTransform->DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void DocScene::NodeSelectionScaleChanged()
{
	if (m_PropTransform) m_PropTransform->DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void DocScene::SelectNodes(const QList<Ogre::SceneNode*>& nodes)
{
	if (!m_Editor) return;

	m_Editor->GetSelection()->StartBulkSelection();
	m_Editor->GetSelection()->Clear(false);
	qforeach (Ogre::SceneNode* node, nodes)
	{
		m_Editor->GetSelection()->AddNode(node);
	}

	m_Editor->GetSelection()->EndBulkSelection();
	m_Editor->UpdateGizmos();
}

//////////////////////////////////////////////////////////////////////////
void DocScene::OnMeshOrSkeletonChanged()
{
	// refresh all properties
	MainWindow::GetInstance()->GetPropWindow()->DisplayData();
	
	// refresh editor selection (the bounding box likely changed)
	m_Editor->GetSelection()->Refresh();

	m_Editor->UpdateGizmos();
}


} // namespace Armed
