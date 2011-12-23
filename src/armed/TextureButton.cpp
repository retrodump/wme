// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "TextureButton.h"
#include "QtUtil.h"
#include "Project.h"
#include "FilePickerDialog.h"


namespace Armed
{


QString TextureButton::allowedTypes = "texture;dds";

//////////////////////////////////////////////////////////////////////////
TextureButton::TextureButton(QWidget* parent) : QPushButton(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	setAcceptDrops(true);

	connect(this, SIGNAL(clicked()), this, SLOT(OnClicked()));
}

//////////////////////////////////////////////////////////////////////////
TextureButton::~TextureButton()
{

}

//////////////////////////////////////////////////////////////////////////
QSize TextureButton::sizeHint() const
{
	return QSize(imageSize + 2 * (border + 1) + arrowSize + 2, imageSize + 2 * (border + 1));
}

//////////////////////////////////////////////////////////////////////////
void TextureButton::paintEvent(QPaintEvent* event)
{
	QPushButton::paintEvent(event);

	QStylePainter painter(this);

	QStyleOption arrowOpt;
	arrowOpt.initFrom(this);
	QPalette pal = arrowOpt.palette;
	pal.setBrush(QPalette::All, QPalette::Text, QColor(0, 0, 0));
	arrowOpt.rect = QRect(width() - arrowSize - border, height() / 2 - arrowSize / 2, arrowSize, arrowSize);
	arrowOpt.palette = pal;
	style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOpt, &painter, this);


	QRect imageRect(border, border, width() - (2 * border + arrowSize) - 3, height() - 2 * border - 1);

	if (!m_Thumbnail.isNull() && m_Thumbnail.hasAlphaChannel())
	{
		painter.drawTiledPixmap(imageRect.adjusted(1, 1, 0, 0), QtUtil::TilePixMap(5));
	}

	painter.setPen(Qt::black);
	painter.setBrush(Qt::transparent);
	painter.drawRect(imageRect);

	if (!m_Thumbnail.isNull())
		painter.drawImage(imageRect.adjusted(1, 1, 0, 0), m_Thumbnail);
}

//////////////////////////////////////////////////////////////////////////
void TextureButton::SetImage(const QString& fileName)
{
	if (fileName == m_FileName) return;

	m_FileName = fileName;
	m_Thumbnail = Project::GetInstance()->GetFileThumbnail(fileName);
	
	if (!m_Thumbnail.isNull()) m_Thumbnail = m_Thumbnail.scaled(imageSize, imageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	setToolTip(m_FileName);	

	emit TextureChanged(m_FileName);
}

//////////////////////////////////////////////////////////////////////////
void TextureButton::OnClicked()
{
	FilePickerDialog dlg(this);
	dlg.SetFilterTypes(allowedTypes);
	dlg.SetCurrentFile(m_FileName);
	if (!dlg.exec()) return;

	SetImage(dlg.GetCurrentFile());
}

//////////////////////////////////////////////////////////////////////////
void TextureButton::dragEnterEvent(QDragEnterEvent* event)
{
	QStringList fileList;
	if (QtUtil::MimeDataToFileList(event->mimeData(), allowedTypes, false, fileList))
	{
		event->setDropAction(Qt::LinkAction);
		event->acceptProposedAction();
	}
}

//////////////////////////////////////////////////////////////////////////
void TextureButton::dropEvent(QDropEvent* event)
{
	QStringList fileList;
	if (QtUtil::MimeDataToFileList(event->mimeData(), allowedTypes, false, fileList))
	{
		SetImage(fileList.at(0));

		event->setDropAction(Qt::LinkAction);
		event->acceptProposedAction();
	}
}


} // namespace Armed
