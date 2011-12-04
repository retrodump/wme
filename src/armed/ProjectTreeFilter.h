// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedProjectTreeFilter_H__
#define __ArmedProjectTreeFilter_H__


#include <QSortFilterProxyModel>


namespace Armed
{
	class ProjectTreeFilter : public QSortFilterProxyModel
	{
		Q_OBJECT

	public:
		ProjectTreeFilter(QObject* parent);
		~ProjectTreeFilter();

		bool filterAcceptsRow(int sourceRow, const QModelIndex& parent) const;

	private:
	
	};
}

#endif // __ArmedProjectTreeFilter_H__
