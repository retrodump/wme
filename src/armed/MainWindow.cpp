// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "MainWindow.h"
#include "Project.h"
#include "DocumentContainer.h"
#include "PaintDispatcher.h"
#include "PropWindow.h"
#include "ProjectBrowser.h"
#include "Navigator.h"
#include "ActionManager.h"
#include "ActionBar.h"
#include "ActionContext.h"


namespace Armed
{


MainWindow* MainWindow::s_Instance = NULL;

//////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
	s_Instance = this;

	this->resize(1020, 661);

	this->setWindowTitle(GetAppTitle());

	m_Project = new Project(this);
	m_PaintDispatcher = new PaintDispatcher(this);
	m_ActionManager = new ActionManager(this);

	// central area
	m_DocContainer = new DocumentContainer(this);
	setCentralWidget(m_DocContainer);

	// right dock
	m_PropWindow = new PropWindow(this);
	QDockWidget* propDock = new QDockWidget(tr("Properties"), this);
	propDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	propDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	propDock->setObjectName(QLatin1String("PropWindow"));
	propDock->setWidget(m_PropWindow);
	addDockWidget(Qt::RightDockWidgetArea, propDock);

	// left dock
	m_ProjectBrowser = new ProjectBrowser(this);
	QDockWidget* projDock = new QDockWidget(tr("Project"), this);
	projDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	projDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	projDock->setObjectName(QLatin1String("ProjectBrowser"));
	projDock->setWidget(m_ProjectBrowser);
	addDockWidget(Qt::LeftDockWidgetArea, projDock);

	m_Navigator = new Navigator(this);
	QDockWidget* navDock = new QDockWidget(tr("Navigator"), this);
	navDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	navDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	navDock->setObjectName(QLatin1String("Navigator"));
	navDock->setWidget(m_Navigator);
	addDockWidget(Qt::LeftDockWidgetArea, navDock);

	tabifyDockWidget(projDock, navDock);
	projDock->raise();


	// status bar
	statusBar()->show();

	BuildActions();


	ReadSettings();

	// setup engine updates
	m_Timer = new QTimer(this);
	m_Timer->setInterval(0);
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(TimerTick()));
	m_Timer->start();

	// temp
	m_Project->Open("../data/project.wpr");
	m_DocContainer->TestData();
}

//////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::TimerTick()
{	
	// updating the game directly here causes hang in Qt 4.7
	// during window resizing/moving
	// we need to schedule the update using the paint event
	m_PaintDispatcher->update();

	//Game::GetInstance()->ProcessFrame();
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent* event)
{
	if (!m_DocContainer->CanClose())
	{
		event->ignore();
		return;
	}

	WriteSettings();
	
	ActionManager::GetInstance()->ClearActionBars();
	event->accept();

	QApplication::quit();
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::BuildActions()
{
	ActionManager* am = ActionManager::GetInstance();

	QAction* actExit = new QAction(tr("Exit"), this);
	actExit->setStatusTip(tr("Exit application"));
	connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
	am->AddAction("File.Exit", actExit, ActionContext::CONTEXT_APP);

	QAction* actOpen = new QAction(tr("Open..."), this);
	actOpen->setStatusTip(tr("Open file"));
	actOpen->setShortcut(QKeySequence::Open);
	actOpen->setIcon(QIcon(":/icons/folder_page.png"));
	connect(actOpen, SIGNAL(triggered()), this, SLOT(ActionTest()));
	am->AddAction("File.Open", actOpen, ActionContext::CONTEXT_APP);


	am->SetContextObject(ActionContext::CONTEXT_APP, this);


	// build menu bar
	ActionBar* menuBar = new ActionBar(this, this->menuBar(), ActionBar::DefineStandardMenu(this));
	am->AddActionBar(menuBar);

	// build main toolbar
	QToolBar* mainToolBar = addToolBar(tr("Main Toolbar"));
	mainToolBar->setObjectName("MainToolbar");
	ActionBar* toolBar = new ActionBar(this, mainToolBar, ActionBar::DefineStandardToolBar(this));
	am->AddActionBar(toolBar);

	am->RefreshActionBars();
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::ActionTest()
{

	ActionManager* am = ActionManager::GetInstance();

	QAction* actNew = new QAction(tr("New"), this);
	actNew->setStatusTip(tr("New document"));
	actNew->setIcon(QIcon(":/icons/page_white_add.png"));
	//connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
	am->AddAction("File.New", actNew, ActionContext::CONTEXT_APP);
/*
	ActionManager* am = ActionManager::GetInstance();
	am->RefreshActionBars();
*/
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::ReadSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value("Geometry").toByteArray());
	restoreState(settings.value("State").toByteArray());
	settings.endGroup();
}

//////////////////////////////////////////////////////////////////////////
void MainWindow::WriteSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("Geometry", saveGeometry());
	settings.setValue("State", saveState());
	settings.endGroup();
}


} // namespace Armed
