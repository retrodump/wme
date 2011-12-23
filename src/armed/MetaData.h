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

		void TouchFile(const QString& fileName);
		QDateTime GetFileLastCheck(const QString& fileName);

		void SaveFileThumbnail(const QString& fileName, const QImage& image);
		QImage LoadFileThumbnail(const QString& fileName);

	private:
		QSqlDatabase m_Db;

		bool BuildDbSchema();
		int GetSchemaVersion() const;
		void SetSchemaVersion(int version);

		bool ReportSqlError(const QSqlQuery& query) const;

		int AddFileToDb(const QString& fileName);
		int GetFileId(const QString& fileName, bool autoCreate = false);
	};
}

#endif // __ArmedMetaData_H__
