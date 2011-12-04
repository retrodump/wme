// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "Project.h"
#include "MetaData.h"
#include "QtUtil.h"
#include "KnownFileType.h"
#include "SystemInfo.h"


using namespace Wme;


namespace Armed
{


Project* Project::s_Instance = NULL;

//////////////////////////////////////////////////////////////////////////
Project::Project(QObject* parent) : QObject(parent)
{
	s_Instance = this;

	RegisterKnownFileTypes();

	m_MetaData = new MetaData(this);
}

//////////////////////////////////////////////////////////////////////////
Project::~Project()
{
	qDeleteAll(m_FileTypes);
}

//////////////////////////////////////////////////////////////////////////
Project* Project::GetInstance()
{
	return s_Instance;
}

//////////////////////////////////////////////////////////////////////////
bool Project::Open(const QString fileName)
{
	Close();

	// TODO ...
	m_FileName = fileName;
	emit ProjectChanged(fileName);
	m_MetaData->Initialize(GetProjectRoot());

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Project::Close()
{
	m_MetaData->Shutdown();
	m_FileName.clear();
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
QString Project::GetProjectRoot() const
{
	return QFileInfo(m_FileName).absoluteDir().path();
}

//////////////////////////////////////////////////////////////////////////
QString Project::GetAbsoluteFileName(const QString& relativeFileName) const
{
	if (QFileInfo(relativeFileName).isAbsolute()) return relativeFileName;

	// TODO packages
	QString absFileName = GetProjectRoot();
	absFileName.append(QDir::separator()).append(relativeFileName);
	return QDir::fromNativeSeparators(absFileName);
}

//////////////////////////////////////////////////////////////////////////
QString Project::GetRelativeFileName(const QString& absoluteFileName) const
{
	QString root = QtUtil::NormalizeFileName(GetProjectRoot()).append(QDir::fromNativeSeparators(QDir::separator()));
	QString fileName = QtUtil::NormalizeFileName(absoluteFileName);

	if (fileName.startsWith(root)) return absoluteFileName.right(absoluteFileName.length() - root.length());
	else return absoluteFileName;
}

//////////////////////////////////////////////////////////////////////////
void Project::RegisterKnownFileTypes()
{
	qDeleteAll(m_FileTypes);
	m_IconLookup.clear();

	KnownFileType* type;

	// textures
	QStringList extList;
	const WideStringSet& imageExtList = SystemInfo::GetSupportedImageFormats();
	foreach (WideString ext, imageExtList)
	{
		extList << QtUtil::WideToQString(ext).mid(1);
	}
	type = new KnownFileType(tr("Textures"), extList.join(";"));
	type->SetIcon(":/icons/picture.png");
	m_FileTypes["texture"] = type;

	// dds textures
	type = new KnownFileType(tr("Compressed textures"), "dds");
	type->SetIcon(":/icons/picture.png");
	m_FileTypes["dds"] = type;

	// sprites
	type = new KnownFileType(tr("Sprites"), "sprite");
	type->SetIcon(":/icons/film.png");
	m_FileTypes["sprite"] = type;

	// scenes
	type = new KnownFileType(tr("Scenes"), "scene");
	type->SetIcon(":/icons/house.png");
	m_FileTypes["scene"] = type;

	// materials
	type = new KnownFileType(tr("Material"), "wmemat");
	type->SetIcon(":/icons/palette.png");
	m_FileTypes["material"] = type;

	// material types
	type = new KnownFileType(tr("Material type"), "matdef");
	type->SetIcon(":/icons/script_palette.png");
	m_FileTypes["matdef"] = type;

	// meshes
	type = new KnownFileType(tr("Meshes"), "mesh");
	type->SetIcon(":/icons/status_offline.png");
	m_FileTypes["mesh"] = type;

	// skeletons
	type = new KnownFileType(tr("Skeletons"), "skeleton");
	type->SetIcon(":/icons/vector.png");
	m_FileTypes["skeleton"] = type;

	// windows
	type = new KnownFileType(tr("Windows"), "window");
	type->SetIcon(":/icons/application_form.png");
	m_FileTypes["window"] = type;

	// fonts
	type = new KnownFileType(tr("Fonts"), "font");
	type->SetIcon(":/icons/font.png");
	m_FileTypes["font"] = type;

	// layouts
	type = new KnownFileType(tr("Layouts"), "layout");
	type->SetIcon(":/icons/layout.png");
	m_FileTypes["layout"] = type;

	// scripts
	type = new KnownFileType(tr("Scripts"), "script;inc");
	type->SetIcon(":/icons/script.png");
	m_FileTypes["script"] = type;

	// sounds
	type = new KnownFileType(tr("Sounds"), "ogg;wav");
	type->SetIcon(":/icons/sound.png");
	m_FileTypes["sound"] = type;

	// videos
	type = new KnownFileType(tr("Videos"), "ogv");
	type->SetIcon(":/icons/film_go.png");
	m_FileTypes["video"] = type;



	// build fast extension->icon lookup table
	qforeach (KnownFileType* type, m_FileTypes)
	{
		if (!type->GetIcon()) continue;

		type->AddIconsToTable(m_IconLookup);
	}
}

//////////////////////////////////////////////////////////////////////////
KnownFileType* Project::GetFileTypeByName(const QString& type) const
{
	if (m_FileTypes.contains(type)) return m_FileTypes[type];
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
KnownFileType* Project::GetFileType(const QString& fileName) const
{
	qforeach (KnownFileType* type, m_FileTypes)
	{
		if (type->MatchesFile(fileName)) return type;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
QIcon* Project::GetFileIcon(const QString& fileName) const
{
	QString ext = QFileInfo(fileName).suffix().toLower();
	if (m_IconLookup.contains(ext)) return m_IconLookup[ext];
	else return NULL;
}


} // namespace Armed
