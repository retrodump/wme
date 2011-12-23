// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "MetaData.h"
#include "Project.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
MetaData::MetaData(QObject* parent) : QObject(parent)
{

}

//////////////////////////////////////////////////////////////////////////
MetaData::~MetaData()
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////
bool MetaData::Initialize(const QString& projectPath)
{
	QString dbPath = projectPath;
	dbPath.append(QDir::separator()).append(".metadata");

	m_Db = QSqlDatabase::addDatabase("QSQLITE");
	m_Db.setDatabaseName(dbPath);

	bool result = m_Db.open();
	if (result)
	{
#if defined(Q_OS_WIN)
		SetFileAttributesW((LPCWSTR)dbPath.utf16(), FILE_ATTRIBUTE_HIDDEN);
#endif
		result = BuildDbSchema();
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////
void MetaData::Shutdown()
{
	if (m_Db.isOpen()) m_Db.close();
}

//////////////////////////////////////////////////////////////////////////
bool MetaData::BuildDbSchema()
{
	if (!m_Db.open()) return false;


	bool result = true;
	QSqlQuery query;

	if (result)
	{
		result = query.exec("CREATE TABLE IF NOT EXISTS file (id INTEGER PRIMARY KEY AUTOINCREMENT, fullName TEXT UNIQUE, shortName TEXT, modTime INTEGER, lastCheck INTEGER)");
	}
	
	if (result)
	{
		result = query.exec("CREATE INDEX IF NOT EXISTS fileNameIndex ON file (fullName)");
	}

	if (result)
	{
		result = query.exec("CREATE TABLE IF NOT EXISTS fileThumbnail (file INTEGER, data BLOB)");
	}

	if (result)
	{
		result = query.exec("CREATE TRIGGER IF NOT EXISTS fileDeleted AFTER DELETE ON file"
							" FOR EACH ROW BEGIN"
							"  DELETE FROM fileThumbnail WHERE file = OLD.id;"
							" END");
	}

#ifdef _DEBUG
	if (!result) QMessageBox::critical(NULL, "Schema error", query.lastError().text());
#endif

	return result;
}

//////////////////////////////////////////////////////////////////////////
int MetaData::AddFileToDb(const QString& fileName)
{
	QSqlQuery query;
	query.prepare("INSERT INTO file (fullName, shortName, modTime, lastCheck) VALUES (?, ?, 0, 0)");
	query.addBindValue(QtUtil::NormalizeFileName(fileName));
	query.addBindValue(QFileInfo(fileName).fileName().toLower());
	query.exec();

	return query.lastInsertId().toInt();
}

//////////////////////////////////////////////////////////////////////////
int MetaData::GetFileId(const QString& fileName, bool autoCreate)
{
	QSqlQuery query;
	query.prepare("SELECT id FROM file WHERE fullName = (?)");
	query.addBindValue(QtUtil::NormalizeFileName(fileName));
	query.exec();

	if (query.next()) return query.value(0).toInt();
	else
	{
		if (autoCreate) return AddFileToDb(fileName);
		else return -1;
	}
}

//////////////////////////////////////////////////////////////////////////
void MetaData::TouchFile(const QString& fileName)
{
	uint currentTime = QDateTime::currentDateTime().toTime_t();

	QString absFileName = Project::GetInstance()->GetAbsoluteFileName(fileName);
	uint fileTime;
	QFileInfo fi = QFileInfo(absFileName);
	if (fi.exists()) fileTime = fi.lastModified().toTime_t();
	else fileTime = 0;

	int id = GetFileId(fileName, true);

	QSqlQuery query;
	query.prepare("UPDATE file SET modTime = ?, lastCheck = ? WHERE id = ?");
	query.addBindValue(fileTime);
	query.addBindValue(currentTime);
	query.addBindValue(id);
	query.exec();
}

//////////////////////////////////////////////////////////////////////////
QDateTime MetaData::GetFileLastCheck(const QString& fileName)
{
	int id = GetFileId(fileName);
	if (id < 0) return QDateTime();
	
	QSqlQuery query;
	query.prepare("SELECT lastCheck FROM file WHERE id = (?)");
	query.addBindValue(id);
	query.exec();

	if (query.next()) return QDateTime::fromTime_t(query.value(0).toInt());
	else return QDateTime();
}

//////////////////////////////////////////////////////////////////////////
void MetaData::SaveFileThumbnail(const QString& fileName, const QImage& image)
{
	int id = GetFileId(fileName);
	if (id < 0) return;

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG");

	QSqlQuery query;
	query.prepare("SELECT file FROM fileThumbnail WHERE file = ?");
	query.addBindValue(id);
	query.exec();

	if (query.next())
	{
		query.prepare("UPDATE fileThumbnail SET data = ?");
		query.addBindValue(ba);
	}
	else
	{
		query.prepare("INSERT INTO fileThumbnail (file, data) VALUES (?, ?)");
		query.addBindValue(id);
		query.addBindValue(ba);
	}
	query.exec();
}

//////////////////////////////////////////////////////////////////////////
QImage MetaData::LoadFileThumbnail(const QString& fileName)
{
	int id = GetFileId(fileName);
	if (id < 0) return QImage();

	QSqlQuery query;
	query.prepare("SELECT data FROM fileThumbnail WHERE file = ?");
	query.addBindValue(id);
	query.exec();

	if (query.next())
	{
		QImage img;
		img.loadFromData(query.value(0).toByteArray());
		return img;
	}
	else return QImage();
}


} // namespace Armed
