// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedProjectTreeModel_H__
#define __ArmedProjectTreeModel_H__


#include <QFileSystemModel>


namespace Armed
{
	class ProjectTreeModel : public QFileSystemModel
	{
		Q_OBJECT

	public:
		ProjectTreeModel(QObject* parent);
		~ProjectTreeModel();

		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	private:
	
	};
}

#endif // __ArmedProjectTreeModel_H__
