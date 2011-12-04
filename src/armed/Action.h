// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedAction_H__
#define __ArmedAction_H__

#include <QObject>
#include "ActionContext.h"

namespace Armed
{
	class Action : public QObject
	{
		Q_OBJECT

	public:
		Action(QObject* parent);
		~Action();

		void SetActionForContext(QAction* action, ActionContext::Type contextType);
		void RemoveActions(QObject* object);

		QAction* GetAction(ActionContext::Type contextType, QObject* object);

	private:
		QMap<ActionContext::Type, ActionContext*> m_Contexts;
	
	};
}

#endif // __ArmedAction_H__
