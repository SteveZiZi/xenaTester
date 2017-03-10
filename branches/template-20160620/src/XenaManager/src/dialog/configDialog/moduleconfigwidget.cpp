#include "moduleconfigwidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include <QLabel>
#include <QBoxLayout>

ModuleConfigWidget::ModuleConfigWidget(CXenaTester *xenaTester,QWidget *parent)
	: BaseConfigWidget(parent),
	m_pXenaTester(xenaTester)
{
	initUI();
}

ModuleConfigWidget::~ModuleConfigWidget()
{
	
}

void ModuleConfigWidget::updateConfigWidget()
{
	setTypeNameLabelContext(m_pXenaTester->chassis()->module(m_currentModuleIndex)->name());
}

void ModuleConfigWidget::initUI()
{
	BaseConfigWidget::initUI();
	setMainLayout();
}

void ModuleConfigWidget::setMainLayout()
{
	QHBoxLayout *topLayout = new QHBoxLayout;
	topLayout->addWidget(m_pNameLabel);
	topLayout->addWidget(m_pTypeNameLabel);
	topLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addSpacing(10);
	mainLayout->addLayout(topLayout);
	mainLayout->addStretch();
	mainLayout->setSpacing(15);
	mainLayout->setMargin(1);

	this->setLayout(mainLayout);
}