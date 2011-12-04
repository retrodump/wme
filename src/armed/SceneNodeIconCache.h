// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedSceneNodeIconCache_H__
#define __ArmedSceneNodeIconCache_H__


#include <QObject>


namespace Armed
{
	class SceneNodeIconCache : public QObject
	{
		Q_OBJECT

	public:
		SceneNodeIconCache(QObject* parent);
		~SceneNodeIconCache();

		static SceneNodeIconCache* GetInstance();

		QIcon* GetIcon(const QString& iconFile);

	private:
		static SceneNodeIconCache* s_Instance;
		QMap<QString, QIcon*> m_IconMap;
	
	};
}

#endif // __ArmedSceneNodeIconCache_H__
