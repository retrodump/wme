// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedMetaData_H__
#define __ArmedMetaData_H__


#include <QObject>


namespace Armed
{
	class MetaData : public QObject
	{
		Q_OBJECT

	public:
		MetaData(QObject* parent);
		~MetaData();

		bool Initialize(const QString& projectPath);
		void Shutdown();

	private:
		QSqlDatabase m_Db;

		bool BuildDbSchema();

		int AddFileToDb(const QString& fileName);
		int GetFileId(const QString& fileName, bool autoCreate = false);
		void TouchFile(const QString& fileName);
		QDateTime GetFileLastCheck(const QString& fileName);

		void SaveFileThumbnail(const QString& fileName, QImage* image);
		QImage* LoadFileThumbnail(const QString& fileName);
	};
}

#endif // __ArmedMetaData_H__
