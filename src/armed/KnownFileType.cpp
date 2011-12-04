// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "KnownFileType.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
KnownFileType::KnownFileType(QObject* parent) : QObject(parent)
{
	m_Icon = NULL;
}

//////////////////////////////////////////////////////////////////////////
KnownFileType::KnownFileType(const QString& name, const QString& extensions, QObject* parent) : QObject(parent)
{
	m_Icon = NULL;
	m_Name = name;

	if (!extensions.isEmpty()) SetExtensions(extensions);
}

//////////////////////////////////////////////////////////////////////////
KnownFileType::~KnownFileType()
{
	SAFE_DELETE(m_Icon);
}

//////////////////////////////////////////////////////////////////////////
void KnownFileType::SetExtensions(const QString& extensions)
{
	QString ext = extensions.toLower();
	m_Extensions = QSet<QString>::fromList(ext.split(";", QString::SkipEmptyParts));
}

//////////////////////////////////////////////////////////////////////////
void KnownFileType::SetIcon(const QString& fileName)
{
	SAFE_DELETE(m_Icon);
	m_Icon = new QIcon(fileName);
}

//////////////////////////////////////////////////////////////////////////
bool KnownFileType::MatchesFile(const QString& fileName)
{
	QString ext = QFileInfo(fileName).suffix().toLower();
	return m_Extensions.contains(ext);
}

//////////////////////////////////////////////////////////////////////////
void KnownFileType::AddIconsToTable(QMap<QString, QIcon*>& table)
{
	if (!m_Icon) return;

	qforeach (const QString& ext, m_Extensions)
	{
		table[ext] = m_Icon;
	}
}


} // namespace Armed
