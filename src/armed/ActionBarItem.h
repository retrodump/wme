// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedActionBarItem_H__
#define __ArmedActionBarItem_H__


#include <QObject>


namespace Armed
{
	class ActionBarItem : public QObject
	{
		Q_OBJECT

	public:
		ActionBarItem(QObject* parent, const QString& actionName = "", const QString& caption = "");
		~ActionBarItem();

		ActionBarItem* AddItem(const QString& actionName = "");
		ActionBarItem* AddSub(const QString& name, const QString& caption);

		const QString& GetName() const { return m_ActionName; }
		const QString& GetCaption() const { return m_Caption; }
		const QList<ActionBarItem*>& GetChildren() const { return m_Children; }

	private:
		QString m_ActionName;
		QString m_Caption;

		QList<ActionBarItem*> m_Children;
	};
}

#endif // __ArmedActionBarItem_H__
