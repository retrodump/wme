// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "ScriptEditor.h"
#include "ScriptHighlighter.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
ScriptEditor::ScriptEditor(QWidget* parent) : QPlainTextEdit(parent)
{
	QFont font;
	font.setFamily("Consolas");
	font.setFixedPitch(true);
	font.setPointSize(11);
	setFont(font);

	QPalette p = palette();
	p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Highlight));
	p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::HighlightedText));
	setPalette(p);

	setWordWrapMode(QTextOption::NoWrap);
	setTabStopWidth(fontMetrics().width(QString(4, '0')));

	m_Highlighter = new ScriptHighlighter(document());
	m_SideBar = new SideBar(this);

	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(UpdateSideBar(QRect, int)));
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(UpdateSideBarWidth()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(UpdateCursor()));

	UpdateSideBarWidth();
}

//////////////////////////////////////////////////////////////////////////
ScriptEditor::~ScriptEditor()
{
}

//////////////////////////////////////////////////////////////////////////
void ScriptEditor::PaintSideBar(QPaintEvent* event)
{
	QPainter painter(m_SideBar);

	const int barWidth = m_SideBar->width();
	const int lineSpacing = painter.fontMetrics().lineSpacing();
	const int fontHeight = painter.fontMetrics().height();

	QTextBlock block = firstVisibleBlock();

	int blockNumber = block.blockNumber();
	int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top;

	int centerOffset = (blockBoundingRect(block).height() - fontHeight) / 2;

	while (block.isValid() && top <= event->rect().bottom())
	{
		top = bottom;
		bottom = top + blockBoundingRect(block).height();

		painter.drawText(0, top + centerOffset, barWidth - 6, fontHeight, Qt::AlignRight, QString::number(blockNumber + 1));

		block = block.next();
		blockNumber = blockNumber + 1;
	}
}

//////////////////////////////////////////////////////////////////////////
void ScriptEditor::UpdateSideBarWidth()
{
	int width = m_SideBar->GetWidth(blockCount());

	if (isLeftToRight())
		setViewportMargins(width, 0, 0, 0);
	else
		setViewportMargins(0, 0, width, 0);
}

//////////////////////////////////////////////////////////////////////////
void ScriptEditor::UpdateSideBar(const QRect& rect, int dy)
{
	if (dy)
		m_SideBar->scroll(0, dy);
	else
		m_SideBar->update(0, rect.y(), m_SideBar->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		UpdateSideBarWidth();
}

//////////////////////////////////////////////////////////////////////////
void ScriptEditor::resizeEvent(QResizeEvent* event)
{
	QPlainTextEdit::resizeEvent(event);

	QRect cr = contentsRect();
	int x = isLeftToRight() ? cr.left() : cr.left() + cr.width() - m_SideBar->GetWidth(blockCount());
	m_SideBar->setGeometry(QRect(x, cr.top(), m_SideBar->GetWidth(blockCount()), cr.height()));
}

//////////////////////////////////////////////////////////////////////////
void ScriptEditor::UpdateCursor()
{
	QTextEdit::ExtraSelection highlight;
	highlight.format.setBackground(QColor(255, 255, 204));
	highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
	highlight.cursor = textCursor();
	highlight.cursor.clearSelection();

	QList<QTextEdit::ExtraSelection> extraSelections;
	extraSelections.append(highlight);

	// todo bracket matching

	setExtraSelections(extraSelections);
}

//////////////////////////////////////////////////////////////////////////
void ScriptEditor::keyPressEvent(QKeyEvent* event)
{
	// ignored keys
	if (event->matches(QKeySequence::NextChild) || event->matches(QKeySequence::PreviousChild))
	{
		event->ignore();
		return;
	}
	// auto indent
	else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
	{		
		QString currLine = textCursor().block().text();
		int indentChars = 0;
		for (int i = 0; i < currLine.length(); i++)
		{
			QChar ch = currLine.at(i);
			if (ch == ' ' || ch == '\t') indentChars++;
			else break;
		}
		textCursor().insertText("\n" + currLine.left(indentChars));
		return;
	}

	QPlainTextEdit::keyPressEvent(event);
}


} // namespace Armed
