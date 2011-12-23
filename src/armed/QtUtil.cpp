// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "QtUtil.h"
#include "StringUtil.h"
#include "Project.h"
#include "KnownFileType.h"
#include "FreeImage.h"

using namespace Wme;


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
WideString QtUtil::QStringToWide(const QString& qString)
{
	// can't use Qt's wide string functions, because Qt is compiled with non-native wchar_t
	// which collides with other libraries
	return StringUtil::Utf8ToWide(qString.toUtf8().constData());
}

//////////////////////////////////////////////////////////////////////////
QString QtUtil::WideToQString(const WideString& wideString)
{
	// can't use Qt's wide string functions, because Qt is compiled with non-native wchar_t
	// which collides with other libraries
	return QString::fromUtf8(StringUtil::WideToUtf8(wideString).c_str());
}

//////////////////////////////////////////////////////////////////////////
Ogre::ColourValue QtUtil::QColorToOgreColor(const QColor& color)
{
	return Ogre::ColourValue(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

//////////////////////////////////////////////////////////////////////////
QColor QtUtil::OgreColorToQColor(const Ogre::ColourValue& ogreColor)
{
	QColor color;
	color.setRgbF(ogreColor.r, ogreColor.g, ogreColor.b, ogreColor.a);
	return color;
}

//////////////////////////////////////////////////////////////////////////
QString QtUtil::NormalizeFileName(const QString& fileName)
{
	return QDir::fromNativeSeparators(fileName.toLower());
}

//////////////////////////////////////////////////////////////////////////
QPixmap QtUtil::TilePixMap(int size)
{
	const int checkerbordSize = size;
	QPixmap tilePixmap(checkerbordSize * 2, checkerbordSize * 2);
	tilePixmap.fill(Qt::white);
	QPainter tilePainter(&tilePixmap);
	QColor color(220, 220, 220);
	tilePainter.fillRect(0, 0, checkerbordSize, checkerbordSize, color);
	tilePainter.fillRect(checkerbordSize, checkerbordSize, checkerbordSize, checkerbordSize, color);
	return tilePixmap;
}

//////////////////////////////////////////////////////////////////////////
bool QtUtil::MimeDataToFileList(const QMimeData* data, const QString& allowedTypes, bool acceptMulti, QStringList& fileList)
{
	if (!data->hasFormat("text/uri-list")) return false;

	const QList<QUrl> list = data->urls();
	if (list.count() == 0) return false;
	if (!acceptMulti && list.count() > 1) return false;

	QSet<QString> extensions;
	FileTypeListToExtList(allowedTypes, extensions);

	for (int i = 0; i < list.count(); i++)
	{
		QString fileName = list.at(i).toLocalFile();
		QFileInfo fi(fileName);

		if (fi.isFile() && extensions.contains(fi.suffix().toLower()))
		{
			fileList.append(Project::GetInstance()->GetRelativeFileName(fileName));
		}
	}

	return !fileList.empty();
}

//////////////////////////////////////////////////////////////////////////
void QtUtil::FileTypeListToExtList(const QString& types, QSet<QString>& extensions)
{
	QStringList typesList = types.split(";", QString::SkipEmptyParts);

	extensions.clear();
	qforeach (const QString& type, typesList)
	{
		KnownFileType* knownType = Project::GetInstance()->GetFileTypeByName(type);
		if (knownType)
		{
			extensions.unite(knownType->GetExtensions());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
QImage QtUtil::LoadImage(const QString& fileName)
{
	QImage image;

	image.load(fileName);
	if (image.isNull())
	{
		// try loading via freeimage
		QByteArray ba = fileName.toLocal8Bit();

		FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileType(ba.data(), 0);
		FIBITMAP* bitmap = FreeImage_Load(imgFormat, ba.data(), 0);
		if (bitmap)
		{
			// create in-memory png
			FIMEMORY* fiMem = FreeImage_OpenMemory();
			FreeImage_SaveToMemory(FIF_PNG, bitmap, fiMem);
			dword size;
			byte* data;
			FreeImage_AcquireMemory(fiMem, &data, &size);

			// let Qt load it
			image.loadFromData(data, size);

			// release memory
			FreeImage_CloseMemory(fiMem);
			FreeImage_Unload(bitmap);
		}
	}
	return image;
}


} // namespace Armed
