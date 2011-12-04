// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedColorButton_H__
#define __ArmedColorButton_H__


#include <QPushButton>


namespace Armed
{
	class ColorButton : public QPushButton
	{
		Q_OBJECT

	public:
		ColorButton(QWidget* parent, const QColor& color = Qt::white);
		~ColorButton();

		QColor GetColor() const { return m_Color; }
		void SetColor(const QColor& color);

		bool GetAlphaChannel() const { return m_AlphaChannel; }
		void SetAlphaChannel(bool alphaChannel);

		bool GetModalDialog() const { return m_ModalDialog; }
		void SetModalDialog(bool modalDialog) { m_ModalDialog = modalDialog; }

		static ColorButton* s_CurrentButton;
		static QColorDialog* s_Dialog;

		void Commit();

	signals:
		void ColorChanged(const QColor& color);

	protected:
		void paintEvent(QPaintEvent* event);

	private slots:
		void OnToggled(bool checked);
		void OnDialogColorChanged(const QColor& color);
		void OnDialogCommitted();
		void OnDialogCancelled();

	private:
		void CloseDialog();
		void ShowDialog();
		void SetTooltip();
		QColor m_Color;
		QColor m_OrigColor;

		bool m_AlphaChannel;
		bool m_ModalDialog;	
	};
}

#endif // __ArmedColorButton_H__
