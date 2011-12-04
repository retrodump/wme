// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropertyFloatWidget.h"
#include "PropertyFloat.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropertyFloatWidget::PropertyFloatWidget(QWidget* parent, PropertyValueFloat* value) : GenericPropertyWidget(parent)
{
	m_Value = value;

	m_SpinBox = new QDoubleSpinBox(this);
	
	if (m_Value->GetParentProp()->GetCheckRange())
	{
		m_SpinBox->setMinimum(m_Value->GetParentProp()->GetMinValue());
		m_SpinBox->setMaximum(m_Value->GetParentProp()->GetMaxValue());
	}
	else
	{
		m_SpinBox->setMinimum(-std::numeric_limits<float>::max());
		m_SpinBox->setMaximum(std::numeric_limits<float>::max());
	}

	m_SpinBox->setDecimals(m_Value->GetParentProp()->GetDecimals());
	m_SpinBox->setKeyboardTracking(false);
	m_SpinBox->setValue(m_Value->GetValue());


	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_SpinBox);

	connect(m_SpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
PropertyFloatWidget::~PropertyFloatWidget()
{

}

//////////////////////////////////////////////////////////////////////////
void PropertyFloatWidget::OnValueChanged(double val)
{
	m_Value->SetValue((float)val);
	emit PropertyChanged();
}


} // namespace Armed
