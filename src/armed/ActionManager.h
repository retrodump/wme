// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedActionManager_H__
#define __ArmedActionManager_H__

#include <QObject>
#include "ActionContext.h"

namespace Armed
{
	class Action;
	class ActionBar;

	class ActionManager : public QObject
	{
		Q_OBJECT
	
	public:
		ActionManager(QObject* parent);
		~ActionManager();

		static ActionManager* GetInstance();

		Action* AddAction(const QString& actionName, QAction* action, ActionContext::Type context);
		void RemoveActions(QObject* object);

		QAction* GetAction(const QString& actionName) const;

		void SetContextObject(ActionContext::Type contextType, QObject* object);

		void AddActionBar(ActionBar* actionBar);

	public slots:
		void RefreshActionBars();
		void ClearActionBars();

	private:
		static ActionManager* s_Instance;

		QMap<QString, Action*> m_NamedActions;
		QMap<ActionContext::Type, QObject*> m_ContextObjects;

		QList<ActionBar*> m_ActionBars;

		bool m_IsDirty;
		void SetDirty(bool dirty);
	};
}

#endif // __ArmedActionManager_H__
