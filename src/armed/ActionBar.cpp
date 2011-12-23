// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ActionBar.h"
#include "ActionManager.h"
#include "ActionBarItem.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ActionBar::ActionBar(QObject* parent, QMenuBar* menuBar, ActionBarItem* items) : QObject(parent)
{
	m_Type = ACTION_BAR_MENUBAR;
	m_MenuBar = menuBar;
	m_Items = items;

	m_Menu = NULL;
	m_ToolBar = NULL;
}

//////////////////////////////////////////////////////////////////////////
ActionBar::ActionBar(QObject* parent, QMenu* menu, ActionBarItem* items) : QObject(parent)
{
	m_Type = ACTION_BAR_MENU;
	m_Menu = menu;
	m_Items = items;

	m_MenuBar = NULL;
	m_ToolBar = NULL;
}

//////////////////////////////////////////////////////////////////////////
ActionBar::ActionBar(QObject* parent, QToolBar* toolBar, ActionBarItem* items) : QObject(parent)
{
	m_Type = ACTION_BAR_TOOBAR;
	m_ToolBar = toolBar;
	m_Items = items;

	m_MenuBar = NULL;
	m_Menu = NULL;
}

//////////////////////////////////////////////////////////////////////////
ActionBar::~ActionBar()
{
}

//////////////////////////////////////////////////////////////////////////
void ActionBar::Rebuild()
{
	switch (m_Type)
	{
	case ACTION_BAR_MENUBAR:
		RebuildMenuBar();
		break;

	case ACTION_BAR_MENU:
		RebuildMenu();
		break;

	case ACTION_BAR_TOOBAR:
		RebuildToolBar();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void ActionBar::Clear()
{
	switch (m_Type)
	{
	case ACTION_BAR_MENUBAR:
		m_MenuBar->clear();
		break;

	case ACTION_BAR_MENU:
		m_Menu->clear();
		break;

	case ACTION_BAR_TOOBAR:
		m_ToolBar->clear();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void ActionBar::RebuildMenuBar()
{
	m_MenuBar->clear();

	ActionManager* am = ActionManager::GetInstance();

	qforeach (ActionBarItem* subMenu, m_Items->GetChildren())
	{
		QMenu* menu = m_MenuBar->addMenu(subMenu->GetCaption());

		bool hasAction = false;
		qforeach (ActionBarItem* item, subMenu->GetChildren())
		{
			if (item->GetName().isEmpty()) menu->addSeparator();
			else
			{
				QAction* action = am->GetAction(item->GetName());
				if (action && action->isVisible())
				{
					hasAction = true;
					menu->addAction(action);
				}
			}
		}
		menu->menuAction()->setVisible(hasAction);
	}
}

//////////////////////////////////////////////////////////////////////////
void ActionBar::RebuildMenu()
{
}

//////////////////////////////////////////////////////////////////////////
void ActionBar::RebuildToolBar()
{
	m_ToolBar->setVisible(false);
	m_ToolBar->clear();

	ActionManager* am = ActionManager::GetInstance();

	bool prevSeparator = false;
	bool hasAction = false;
	qforeach (ActionBarItem* item, m_Items->GetChildren())
	{
		if (item->GetName().isEmpty())
		{
			prevSeparator = true;
			continue;
		}

		QAction* action = am->GetAction(item->GetName());
		if (action && action->isVisible())
		{
			if (prevSeparator && hasAction) m_ToolBar->addSeparator();
			prevSeparator = false;
			hasAction = true;

			m_ToolBar->addAction(action);
		}
	}
	m_ToolBar->setVisible(hasAction);
}

//////////////////////////////////////////////////////////////////////////
ActionBarItem* ActionBar::DefineStandardMenu(QObject* parent)
{
	ActionBarItem* root = new ActionBarItem(parent);

	ActionBarItem* fileMenu = root->AddSub("FileMenu", tr("File"));
	fileMenu->AddItem("File.New");
	fileMenu->AddItem("File.Open");
	fileMenu->AddItem();
	fileMenu->AddItem("File.Save");
	fileMenu->AddItem("File.SaveAs");
	fileMenu->AddItem();
	fileMenu->AddItem("File.Exit");

	ActionBarItem* editMenu = root->AddSub("EditMenu", tr("Edit"));
	editMenu->AddItem("Edit.Find");
	editMenu->AddItem("Edit.FindNext");
	editMenu->AddItem("Edit.FindPrev");

	ActionBarItem* sceneMenu = root->AddSub("SceneMenu", tr("Scene"));
	sceneMenu->AddItem("Scene.Transform.Select");
	sceneMenu->AddItem("Scene.Transform.Move");
	sceneMenu->AddItem("Scene.Transform.Rotate");
	sceneMenu->AddItem("Scene.Transform.Scale");

	return root;
}

//////////////////////////////////////////////////////////////////////////
ActionBarItem* ActionBar::DefineStandardToolBar(QObject* parent)
{
	ActionBarItem* root = new ActionBarItem(parent);

	root->AddItem("File.New");
	root->AddItem("File.Open");
	root->AddItem();
	root->AddItem("File.Save");
	root->AddItem();
	root->AddItem("Edit.Find");
	root->AddItem();
	root->AddItem("File.Exit");
	root->AddItem();

	return root;
}


} // namespace Armed
