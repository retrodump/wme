// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedMainWindow_H__
#define __ArmedMainWindow_H__

#include <QMainWindow>

namespace Armed
{
	class DocumentContainer;
	class PaintDispatcher;
	class PropWindow;
	class ProjectBrowser;
	class Navigator;
	class ActionManager;
	class Project;

	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
		~MainWindow();

		static QString GetAppTitle() { return tr("Armitage Editor"); }

		static MainWindow* GetInstance() { return s_Instance; }

		Navigator* GetNavigator() const { return m_Navigator; }
		DocumentContainer* GetDocContainer() const { return m_DocContainer; }
		ProjectBrowser* GetProjectBrowser() const { return m_ProjectBrowser; }
		PropWindow* GetPropWindow() const { return m_PropWindow; }

	private slots:
		void TimerTick();
		void ActionTest();

	private:
		static MainWindow* s_Instance;

		Project* m_Project;
		PaintDispatcher* m_PaintDispatcher;
		ActionManager* m_ActionManager;
		QTimer* m_Timer;

		DocumentContainer* m_DocContainer;
		PropWindow* m_PropWindow;
		ProjectBrowser* m_ProjectBrowser;
		Navigator* m_Navigator;

		void BuildActions();

		void ReadSettings();
		void WriteSettings();

		void closeEvent(QCloseEvent* event);
	};
}

#endif // __ArmedMainWindow_H__
