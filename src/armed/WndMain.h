// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedWndMain_H__
#define __ArmedWndMain_H__

#include <QtGui/QMainWindow>
#include "ui_WndMain.h"


namespace Armed
{
	class WndMain : public QMainWindow
	{
		Q_OBJECT

	public:
		WndMain(QWidget* parent = 0, Qt::WFlags flags = 0);
		~WndMain();

	private slots:
		void TimerTick();

	private:	
		Ui::WndMainClass ui;
		QTimer* m_Timer;
	};
}

#endif // __ArmedWndMain_H__
