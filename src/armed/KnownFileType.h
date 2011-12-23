// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedKnownFileType_H__
#define __ArmedKnownFileType_H__


#include <QObject>


namespace Armed
{
	class KnownFileType : public QObject
	{
		Q_OBJECT

	public:
		KnownFileType(QObject* parent = NULL);
		KnownFileType(const QString& type, const QString& name, const QString& extensions = "", QObject* parent = NULL);
		~KnownFileType();

		void SetType(const QString& type) { m_Type = type; }
		QString GetType() const { return m_Type; }

		void SetName(const QString& name) { m_Name = name; }
		QString GetName() const { return m_Name; }

		void SetExtensions(const QString& extensions);
		const QSet<QString>& GetExtensions() const { return m_Extensions; }

		void SetIcon(const QString& fileName);
		QIcon* GetIcon() const { return m_Icon; }

		bool MatchesFile(const QString& fileName);

		void AddIconsToTable(QMap<QString, QIcon*>& table);


	private:
		QString m_Type;
		QString m_Name;
		QSet<QString> m_Extensions;
		QIcon* m_Icon;
	
	};
}

#endif // __ArmedKnownFileType_H__
