// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "stdafx.h"
#include "MainWindow.h"
#include "Game.h"
#include <QtGui/QApplication>
#include "WmeWidget.h"
#include "EmbeddedWindow.h"
#include "QtWin.h"


#if defined(_MSC_VER)
#	ifdef _DEBUG
#		include <vld.h>
#	endif
#endif


using namespace Armed;
using namespace Wme;

//////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int ret;
	QApplication app(argc, argv);

	app.setOrganizationName("Dead:Code");
	app.setOrganizationDomain("dead-code.org");
	app.setApplicationName("Armitage Editor");


	Game* Game = Game::GetInstance();
	if (Game->Initialize(false))
	{
		// it's necessary that the primary OpenGL context is destroyed last or Ogre crashes
		// that's we have to create a dummy Wme view first
		WmeWidget* dummyView = new WmeWidget(NULL);
		dummyView->setObjectName("primaryEmbeddedWindow");
		dummyView->CreateWmeView();
		dummyView->GetWmeView()->SetTopLevelWindow(false);
		dummyView->hide();

		// create main window and start the app
		MainWindow* mainWindow = new MainWindow(NULL);

		mainWindow->show();
		ret = app.exec();

		// destroy the main window together with all child Wme views
		delete mainWindow;

		// and eventually destroy the primary Wme view
		delete dummyView;
	}
	else ret = 1;

	delete Game;
	
	return ret;
}

