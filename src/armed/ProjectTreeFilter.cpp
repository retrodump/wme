// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ProjectTreeFilter.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ProjectTreeFilter::ProjectTreeFilter(QObject* parent) : QSortFilterProxyModel(parent)
{

}

ProjectTreeFilter::~ProjectTreeFilter()
{

}

//////////////////////////////////////////////////////////////////////////
bool ProjectTreeFilter::filterAcceptsRow(int sourceRow, const QModelIndex& parent) const
{
	return QSortFilterProxyModel::filterAcceptsRow(sourceRow, parent);
}


} // namespace Armed