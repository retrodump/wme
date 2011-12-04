// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedActionContext_H__
#define __ArmedActionContext_H__

#include <QObject>


namespace Armed
{
	class ActionContext : public QObject
	{
		Q_OBJECT

	public:
		ActionContext(QObject* parent);
		~ActionContext();

		enum Type
		{
			CONTEXT_APP = 0,
			CONTEXT_DOC = 1,
			CONTEXT_WIDGET = 2,
			NUM_CONTEXTS
		};

		void SetAction(QAction* action);
		void RemoveActions(QObject* object);

		QAction* GetAction(QObject* object);

	private:
		QMap<QObject*, QAction*> m_ActionMap;
	
	};
}


#endif // __ArmedActionContext_H__
