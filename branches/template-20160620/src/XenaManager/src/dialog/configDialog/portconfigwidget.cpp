#include "portconfigwidget.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include "XenaManager/src/treeView/ChassisViewWidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/deviceImpl.h"
#include "XenaManager/src/Communicate/port.h"
#include "XenaManager/src/Communicate/observer.h"
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QTimeEdit>
#include <QComboBox>
#include <QListView>
#include <QBoxLayout>
#include <QMessageBox>

enum E_HEADERLENGTH{
	HEADERLENGTH1024,
	HEADERLENGTH512,
	HEADERLENGTH256,
	HEADERLENGTH128
};

PortConfigWidget::PortConfigWidget(CXenaTester * xenaTester,ChassisViewWidget * chassisView,QWidget *parent)
	:BaseConfigWidget(parent),
	m_pXenaTester(xenaTester),
    m_pChassisViewWidget(chassisView),
    m_portObserver(new CPortObserver(xenaTester, this))
{
	initUI();
	// 本地
	connect(m_pHeadLengthComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
	connect(m_pKeepTimeEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
	connect(m_pStartBtn,SIGNAL(clicked()),this,SLOT(onStartBtn()));
	connect(m_pStopBtn,SIGNAL(clicked()),this,SLOT(onStopBtn()));
}

PortConfigWidget::~PortConfigWidget()
{
    
}

void PortConfigWidget::updateConfigWidget()
{
    m_pPort = m_pXenaTester->chassis()->module(m_currentModuleIndex)->port(m_currentPortIndex);
    setTypeNameLabelContext(m_pPort->portName() + " " + QString::number(m_pPort->realSpeed()) + "M");
    
    disconnect(m_pChassisViewWidget,SIGNAL(updatePortTraffic(int,int)),this,SLOT(updateTraffic(int,int)));

    setControlEnable(false);
    m_pStopBtn->setEnabled(false);

    if (m_pPort->reserved())
    {
        if (QString(LoginSettings::getUserName().data()) == m_pPort->reservedBy())
        {
            setControlEnable(true);
            m_pStopBtn->setEnabled(true);

            updateTrafficStatus();
            updateHeadLength();
            updateTxTimeLimit();

            connect(m_pChassisViewWidget,SIGNAL(updatePortTraffic(int,int)),this,SLOT(updateTraffic(int,int)));
        }
    }
}

void PortConfigWidget::initUI()
{
	m_trafficed = false;

	BaseConfigWidget::initUI();

	m_pHeaderLengthLabel = new QLabel(tr("帧头长度："));
	m_pKeepTimeLabel = new QLabel(tr("保持时间："));

	m_pHeadLengthComBox = new QComboBox;
    m_pHeadLengthComBox->setView(new QListView());
	m_pHeadLengthComBox->addItem("1024bytes");
	m_pHeadLengthComBox->addItem("512bytes");
	m_pHeadLengthComBox->addItem("256bytes");
	m_pHeadLengthComBox->addItem("128bytes");

	m_pStartBtn = new QPushButton(tr("开始"));
	m_pStopBtn = new QPushButton(tr("停止"));

	m_pKeepTimeEdit = new QTimeEdit;
	m_pKeepTimeEdit->setEnabled(false);

	setMainLayout();
}

void PortConfigWidget::onCurrentIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    int headLength;
    switch (index)
    {
    case HEADERLENGTH1024:
        headLength = 1024;
        break;
    case HEADERLENGTH512:
        headLength = 512;
        break;
    case HEADERLENGTH256:
        headLength = 256;
        break;
    case HEADERLENGTH128:
        headLength = 128;
        break;
    default:
        break;
    }

    m_pXenaTester->device()->setPortMaxHeaderLength(m_currentModuleIndex,m_currentPortIndex,headLength);
    m_pPort->setMaxHeaderLength(headLength);
}

void PortConfigWidget::onEditingFinished()
{
    QTime time = m_pKeepTimeEdit->time();
    int millisecond = time.second() * 1000;

    m_pXenaTester->device()->setPortTimeLimit(m_currentModuleIndex,m_currentPortIndex,millisecond);
    m_pPort->setTxTimeLimit(millisecond);
}

void PortConfigWidget::onStartBtn()
{
    if (0 == m_pPort->streamCount()) {
        QMessageBox::information(this, tr("提示"), tr("未添加流"),QString(tr("确定")));
        return ;
    }

    bool addObserve = false;
    for(int i = 0; i < m_pPort->streamCount(); i++) {
        CStream*stream = m_pPort->stream(i);
        if (stream->strategy() != CStream::STRATEGY_NONE) {
            addObserve = true;
            break;
        }
    }
    m_pXenaTester->portStartSpeedTraffic(m_pPort);
    if (addObserve) {
        m_portObserver->observePort(m_pPort);
    }

    setControlEnable(false);
}

void PortConfigWidget::onStopBtn()
{
    m_portObserver->removePort(m_pPort);
    m_pXenaTester->portStopTranffic(m_pPort);

    setControlEnable(true);
}

void PortConfigWidget::updateTraffic(int moduleIndex,int portIndex)
{
    if ((moduleIndex == m_currentModuleIndex) && 
        (portIndex == m_currentPortIndex))
    {
        updateTrafficStatus();
    }
}

void PortConfigWidget::setMainLayout()
{
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(m_pNameLabel);
    nameLayout->addWidget(m_pTypeNameLabel);
    nameLayout->addStretch();

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addLayout(nameLayout);

    QHBoxLayout *keepTimeLayout = new QHBoxLayout;
    keepTimeLayout->addWidget(m_pKeepTimeLabel);
    keepTimeLayout->addWidget(m_pKeepTimeEdit);
    keepTimeLayout->addStretch();

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addWidget(m_pHeaderLengthLabel);
    middleLayout->addWidget(m_pHeadLengthComBox);
    middleLayout->addStretch();

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(m_pStartBtn);
    bottomLayout->addWidget(m_pStopBtn);
    bottomLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(middleLayout);
    mainLayout->addLayout(keepTimeLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addStretch();
    mainLayout->setSpacing(15);
    mainLayout->setMargin(1);

    this->setLayout(mainLayout);
}

void PortConfigWidget::setControlEnable(bool state)
{
	m_pStartBtn->setEnabled(state);
	m_pHeadLengthComBox->setEnabled(state);
    m_pKeepTimeEdit->setEnabled(state);
}

void PortConfigWidget::updateHeadLength()
{
    int headLength = m_pPort->maxHeaderLength();

    switch(headLength)
    {
    case 1024:
        m_pHeadLengthComBox->setCurrentIndex(HEADERLENGTH1024);
        break;
    case 512:
        m_pHeadLengthComBox->setCurrentIndex(HEADERLENGTH512);
        break;
    case 256:
        m_pHeadLengthComBox->setCurrentIndex(HEADERLENGTH256);
        break;
    case 128:
        m_pHeadLengthComBox->setCurrentIndex(HEADERLENGTH128);
        break;
    default:
        break;
    }
}

void PortConfigWidget::updateTrafficStatus()
{
    m_trafficed = m_pPort->actived();
    setControlEnable(!m_trafficed);
}

void PortConfigWidget::updateTxTimeLimit()
{
    int millisecond = m_pPort->txTimeLimit();

    QTime time;
    m_pKeepTimeEdit->setTime(time.addMSecs(millisecond));
}

#if 0
void PortConfigWidget::startStrategy()
{
	S_ORDER order;
	order.moduleIndex = m_currentModuleIndex;
	order.portIndex = m_currentPortIndex;
	for (int streamCount = 0;streamCount < m_pPortStrategy->streamStrategy.size();streamCount++)
	{
		order.streamindex = streamCount;
		// 流不使能的情况
		if(!m_pPortStrategy->streamStrategy[streamCount]->streamEnable)
		{
			continue;
		}
		else
		{
			// 没选策略
			if(!m_pPortStrategy->streamStrategy[streamCount]->stepStrategyEnable)
			{
				continue;
			}
			else
			{
				// 帧数量策略
				if (m_pPortStrategy->streamStrategy[streamCount]->packetStrategyEnable)
				{
					setLoopStratgy(m_pPortStrategy->streamStrategy[streamCount],order);
					continue;
				}
				// 时间策略
				else if (m_pPortStrategy->streamStrategy[streamCount]->packetStrategyEnable)
				{
					setLoopStratgy(m_pPortStrategy->streamStrategy[streamCount],order);
// 					for (int loopTimes = 0;loopTimes < m_pPortStrategy->streamStrategy[streamCount]->loopTimes;loopTimes++)
// 					{
// 						double currentRate = m_pPortStrategy->streamStrategy[streamCount]->beginRate;
// 						while(currentRate < m_pPortStrategy->streamStrategy[streamCount]->endRate)
// 						{
// 							order.data = QString::number(currentRate * 10000);
// 							m_pCommunicate->sendData(PS_RATEFRACTION_SET,order);
// 							if (false == m_trafficed)
// 							{
// 								currentRate += m_pPortStrategy->streamStrategy[streamCount]->stepRate;
// 								if (currentRate > 100.0)
// 								{
// 									currentRate = 100.0;
// 								}
// 								m_pCommunicate->sendData(P_TRAFFIC_ON,order);
// 							}
// 						}
// 					}
				}
			}
		}

	}
}

void PortConfigWidget::setLoopStratgy(StreamStrategy *streamStrategy,S_ORDER & order)
{
	for (int loopTimes = 0;loopTimes < streamStrategy->loopTimes;loopTimes++)
	{
		double currentRate = streamStrategy->beginRate;
		while(currentRate < streamStrategy->endRate)
		{
			order.data = QString::number(currentRate * 10000);
			m_pCommunicate->sendData(PS_RATEFRACTION_SET,order);
			
			if (false == m_trafficed)
			{
				currentRate += streamStrategy->stepRate;
				if (currentRate > 100.0)
				{
					currentRate = 100.0;
				}
				m_pCommunicate->sendData(P_TRAFFIC_ON,order);
			}
		}
	}
}

#endif