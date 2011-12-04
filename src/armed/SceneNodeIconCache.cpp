// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "SceneNodeIconCache.h"
#include "MainWindow.h"


namespace Armed
{


SceneNodeIconCache* SceneNodeIconCache::s_Instance = NULL;

//////////////////////////////////////////////////////////////////////////
SceneNodeIconCache::SceneNodeIconCache(QObject* parent) : QObject(parent)
{
}

//////////////////////////////////////////////////////////////////////////
SceneNodeIconCache::~SceneNodeIconCache()
{
	qDeleteAll(m_IconMap);
}

//////////////////////////////////////////////////////////////////////////
SceneNodeIconCache* SceneNodeIconCache::GetInstance()
{
	if (!s_Instance)
	{
		s_Instance = new SceneNodeIconCache(MainWindow::GetInstance());
	}
	return s_Instance;
}

//////////////////////////////////////////////////////////////////////////
QIcon* SceneNodeIconCache::GetIcon(const QString& iconFile)
{
	if (m_IconMap.contains(iconFile)) return m_IconMap[iconFile];
	else
	{
		QIcon* newIcon = new QIcon(iconFile);

		// store icon in cache
		m_IconMap[iconFile] = newIcon;
		return newIcon;
	}
}


} // namespace Armed