// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ProjectTreeModel.h"
#include "Project.h"
#include "KnownFileType.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ProjectTreeModel::ProjectTreeModel(QObject* parent) : QFileSystemModel(parent)
{

}

//////////////////////////////////////////////////////////////////////////
ProjectTreeModel::~ProjectTreeModel()
{

}

//////////////////////////////////////////////////////////////////////////
int ProjectTreeModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////
QVariant ProjectTreeModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid())
	{
		if (role == Qt::ToolTipRole)
			return QDir::toNativeSeparators(Project::GetInstance()->GetRelativeFileName(filePath(index)));

		// override icons of known file types
		if (role == Qt::DecorationRole && index.column() == 0)
		{
			QString fileName = filePath(index);
			if (!QFileInfo(fileName).isDir())
			{
				QIcon* icon = Project::GetInstance()->GetFileIcon(fileName);
				if (icon) return *icon;
			}
		}
	}

	return QFileSystemModel::data(index, role);
}


} // namespace Armed
