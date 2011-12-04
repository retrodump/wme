// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "FileButton.h"
#include "FilePickerDialog.h"
#include "Project.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
FileButton::FileButton(QWidget* parent, const QString& fileName) : QPushButton(parent)
{
	setAcceptDrops(true);
	m_FileName = fileName;
	m_PlaceholderText = "";

	connect(this, SIGNAL(clicked()), this, SLOT(OnClicked()));
}

//////////////////////////////////////////////////////////////////////////
FileButton::~FileButton()
{

}

//////////////////////////////////////////////////////////////////////////
void FileButton::SetFileName(const QString& fileName)
{
	m_FileName = fileName;
	setToolTip(m_FileName);	
	update();
	emit FileChanged(m_FileName);
}

//////////////////////////////////////////////////////////////////////////
void FileButton::SetPlaceholderText(const QString& text)
{
	m_PlaceholderText = text;
	if (m_FileName.isEmpty()) update();
}

//////////////////////////////////////////////////////////////////////////
void FileButton::OnClicked()
{
	FilePickerDialog dlg(this);
	dlg.SetFilterTypes(m_AllowedTypes);
	dlg.SetCurrentFile(m_FileName);
	if (!dlg.exec()) return;

	SetFileName(dlg.GetCurrentFile());
}

//////////////////////////////////////////////////////////////////////////
void FileButton::paintEvent(QPaintEvent* event)
{
	QPushButton::paintEvent(event);

	QStylePainter painter(this);

	int arrowSize = 10;
	int border = 4;

	QStyleOption arrowOpt;
	arrowOpt.initFrom(this);
	QPalette pal = arrowOpt.palette;
	pal.setBrush(QPalette::All, QPalette::Text, QColor(0, 0, 0));
	arrowOpt.rect = QRect(width() - arrowSize - border, height() / 2 - arrowSize / 2, arrowSize, arrowSize);
	arrowOpt.palette = pal;
	style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOpt, &painter, this);

	QString displayText = "";

	if (!m_FileName.isEmpty())
	{
		// paint non-existent files in red
		QString fullFileName = Project::GetInstance()->GetAbsoluteFileName(m_FileName);
		if (!QFileInfo(fullFileName).exists()) painter.setPen(Qt::red);
		displayText = m_FileName;
	}
	else if (!m_PlaceholderText.isEmpty())
	{
		displayText = "(" + m_PlaceholderText + ")";
		//painter.setPen(Qt::darkGray);
	}

	if (!displayText.isEmpty())
	{
		QRect nameRect(border, border, width() - (3 * border + arrowSize) - 1, height() - 2 * border - 1);

		QFontMetrics fm(font());
		QString elidedText = fm.elidedText(displayText, Qt::ElideLeft, nameRect.width());
	
		painter.drawText(nameRect, Qt::AlignCenter, elidedText);
	}
}

//////////////////////////////////////////////////////////////////////////
void FileButton::dragEnterEvent(QDragEnterEvent* event)
{
	QStringList fileList;
	if (QtUtil::MimeDataToFileList(event->mimeData(), m_AllowedTypes, false, fileList))
	{
		event->setDropAction(Qt::LinkAction);
		event->acceptProposedAction();
	}
}

//////////////////////////////////////////////////////////////////////////
void FileButton::dropEvent(QDropEvent* event)
{
	QStringList fileList;
	if (QtUtil::MimeDataToFileList(event->mimeData(), m_AllowedTypes, false, fileList))
	{
		SetFileName(fileList.at(0));

		event->setDropAction(Qt::LinkAction);
		event->acceptProposedAction();
	}
}


} // namespace Armed
