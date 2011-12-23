// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedProject_H__
#define __ArmedProject_H__


#include <QObject>


namespace Armed
{
	class MetaData;
	class KnownFileType;

	class Project : public QObject
	{
		Q_OBJECT

	public:
		Project(QObject* parent);
		~Project();

		static Project* GetInstance();

		bool Open(const QString fileName);
		bool Close();

		QString GetFileName() const { return m_FileName; }
		QString GetProjectRoot() const;

		QString GetAbsoluteFileName(const QString& relativeFileName) const;
		QString GetRelativeFileName(const QString& absoluteFileName) const;

		MetaData* GetMetaData() const { return m_MetaData; }


		KnownFileType* GetFileTypeByName(const QString& type) const;
		KnownFileType* GetFileType(const QString& fileName) const;
		QIcon* GetFileIcon(const QString& fileName) const;

		QImage GetFileThumbnail(const QString& fileName) const;

	signals:
		void ProjectChanged(const QString& fileName);

	private:
		mutable MetaData* m_MetaData;

		static Project* s_Instance;

		QString m_FileName;
		
		void RegisterKnownFileTypes();
		QMap<QString, KnownFileType*> m_FileTypes;
		QMap<QString, QIcon*> m_IconLookup;

		QImage GenerateThumbnail(const QString& fileName) const;
	};
}

#endif // __ArmedProject_H__
