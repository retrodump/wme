// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedActionBar_H__
#define __ArmedActionBar_H__


#include <QObject>


namespace Armed
{
	class ActionBarItem;

	class ActionBar : public QObject
	{
		Q_OBJECT

	public:
		ActionBar(QObject* parent, QMenuBar* menuBar, ActionBarItem* items);
		ActionBar(QObject* parent, QMenu* menu, ActionBarItem* items);
		ActionBar(QObject* parent, QToolBar* toolBar, ActionBarItem* items);
		~ActionBar();

		void Rebuild();
		void Clear();

		static ActionBarItem* DefineStandardMenu(QObject* parent);
		static ActionBarItem* DefineStandardToolBar(QObject* parent);

		enum Type
		{
			ACTION_BAR_MENUBAR,
			ACTION_BAR_MENU,
			ACTION_BAR_TOOBAR
		};

	private:
		ActionBarItem* m_Items;
		Type m_Type;
		QMenuBar* m_MenuBar;
		QMenu* m_Menu;
		QToolBar* m_ToolBar;

		void RebuildMenuBar();
		void RebuildMenu();
		void RebuildToolBar();
	};
}

#endif // __ArmedActionBar_H__
