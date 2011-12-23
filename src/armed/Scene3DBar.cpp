// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "Scene3DBar.h"
#include "DocScene.h"
#include "ActionManager.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
Scene3DBar::Scene3DBar(DocScene* parent) : QWidget(parent)
{
	m_Ui.setupUi(this);

	m_Scene = parent;
}

//////////////////////////////////////////////////////////////////////////
Scene3DBar::~Scene3DBar()
{

}

//////////////////////////////////////////////////////////////////////////
void Scene3DBar::Update()
{
	ActionManager* am = ActionManager::GetInstance();

	m_Ui.SelectionButton->setDefaultAction(am->GetAction("Scene.Transform.Select"));
	m_Ui.MoveButton->setDefaultAction(am->GetAction("Scene.Transform.Move"));
	m_Ui.RotateButton->setDefaultAction(am->GetAction("Scene.Transform.Rotate"));
	m_Ui.ScaleButton->setDefaultAction(am->GetAction("Scene.Transform.Scale"));

	m_Ui.LocalButton->setDefaultAction(am->GetAction("Scene.GizmoMode.Local"));
	m_Ui.WorldButton->setDefaultAction(am->GetAction("Scene.GizmoMode.World"));
}


} // namespace Armed
