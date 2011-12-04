// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "stdafx.h"
#include "WndMain.h"
#include "Game.h"
#include "WmeWidget.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
WndMain::WndMain(QWidget* parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
	ui.setupUi(this);

	m_Timer = new QTimer(this);
	m_Timer->setInterval(0);
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(TimerTick()));
	m_Timer->start();


	/*
	ui.content->CreateWmeView();
	ui.scroll->CreateWmeView();

	ui.content->setVisible(false);
	ui.scroll->setVisible(false);

	QPushButton* b = new QPushButton(this);
	b->setText("meh");
	b->setMinimumHeight(100);
	b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QPushButton* b2 = new QPushButton(this);
	b2->setText("bah");
	b2->setMinimumHeight(200);

	QPushButton* b3 = new QPushButton(this);
	b3->setText("blah");
	b3->setMinimumHeight(200);

	ui.propBar->AddItem(b, "Placement");
	ui.propBar->AddItem(b2, "Material", false);
	ui.propBar->AddItem(b3, "Scripts");
	*/
}

//////////////////////////////////////////////////////////////////////////
WndMain::~WndMain()
{
	SAFE_DELETE(m_Timer);
}

//////////////////////////////////////////////////////////////////////////
void WndMain::TimerTick()
{
	Game::GetInstance()->ProcessFrame();
}


} // namespace Armed

