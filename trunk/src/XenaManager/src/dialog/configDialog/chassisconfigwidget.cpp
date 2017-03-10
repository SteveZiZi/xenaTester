#include "chassisconfigwidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include <QLabel>
#include <QBoxLayout>


ChassisConfigWidget::ChassisConfigWidget(CXenaTester *xenaTester,QWidget *parent)
    :BaseConfigWidget(parent),
    m_pXenaTester(xenaTester)
{
    initUI();
}

ChassisConfigWidget::~ChassisConfigWidget()
{
	
}

void ChassisConfigWidget::updateConfigWidget()
{
    setTypeNameLabelContext(m_pXenaTester->chassis()->chassisName());
}

void ChassisConfigWidget::initUI()
{
    BaseConfigWidget::initUI();
    setMainLayout();
}

void ChassisConfigWidget::setMainLayout()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(m_pNameLabel);
    topLayout->addWidget(m_pTypeNameLabel);
    topLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(15);

    this->setLayout(mainLayout);
}
