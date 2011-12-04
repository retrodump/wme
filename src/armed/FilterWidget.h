// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedFilterWidget_H__
#define __ArmedFilterWidget_H__


#include <QWidget>


namespace Armed
{
	//////////////////////////////////////////////////////////////////////////
	class IconButton: public QToolButton
	{
		Q_OBJECT
	public:
		IconButton(QWidget* parent);
		void paintEvent(QPaintEvent* event);
		
		static const int s_ButtonSize = 16;
	};


	//////////////////////////////////////////////////////////////////////////
	class FilterWidget : public QWidget
	{
		Q_OBJECT

	public:
		FilterWidget(QWidget* parent);
		~FilterWidget();

		QString text() const;
		void resizeEvent(QResizeEvent*);
		void keyPressEvent(QKeyEvent* e);

	signals:
		void FilterChanged(const QString&);

	public slots:
		void Reset();

	private slots:
		void CheckButton(const QString& text);

	private:
		QLineEdit* m_Editor;
		IconButton* m_ClearButton;

		QString m_OldText;

		
	};
}

#endif // __ArmedFilterWidget_H__
