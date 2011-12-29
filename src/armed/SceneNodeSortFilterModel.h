// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedSceneNodeSortFilterModel_H__
#define __ArmedSceneNodeSortFilterModel_H__


#include <QSortFilterProxyModel>


namespace Armed
{
	class SceneNodeSortFilterModel : public QSortFilterProxyModel
	{
		Q_OBJECT

	public:
		SceneNodeSortFilterModel(QObject* parent);
		~SceneNodeSortFilterModel();

		bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
		bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

	private:
		bool MatchChildren(const QModelIndex& parent) const;
	
	};
}

#endif // __ArmedSceneNodeSortFilterModel_H__
