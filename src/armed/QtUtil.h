// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeQtUtil_H__
#define __WmeQtUtil_H__

using namespace Wme;

namespace Armed
{
	class QtUtil
	{
	public:
		static WideString QStringToWide(const QString& qString);
		static QString WideToQString(const WideString& wideString);

		static Ogre::ColourValue QColorToOgreColor(const QColor& color);
		static QColor OgreColorToQColor(const Ogre::ColourValue& ogreColor);

		static QString NormalizeFileName(const QString& fileName);

		static QPixmap TilePixMap(int size);

		static bool MimeDataToFileList(const QMimeData* data, const QString& allowedTypes, bool acceptMulti, QStringList& fileList);
		static void FileTypeListToExtList(const QString& types, QSet<QString>& extensions);

		static QImage LoadImage(const QString& fileName);
	};
}

#endif // __WmeQtUtil_H__