#include "baseconfigwidget.h"
#include <QLabel>
#include <QBoxLayout>
#include <QDebug>


BaseConfigWidget::BaseConfigWidget(QWidget *parent)
	: QWidget(parent)
{
	
}

BaseConfigWidget::~BaseConfigWidget()
{
	
}

void BaseConfigWidget::setTypeNameLabelContext(const QString & name)
{
	m_pTypeNameLabel->setText(name);
}

void BaseConfigWidget::setCurrentIndex(int module,int port,int stream)
{
	m_currentModuleIndex = module;
	m_currentPortIndex = port;
	m_currentStreamIndex = stream;
}

void BaseConfigWidget::initUI()
{
	m_pNameLabel = new QLabel(tr("Ãû³Æ£º"),this);
	m_pTypeNameLabel = new QLabel(this);
}