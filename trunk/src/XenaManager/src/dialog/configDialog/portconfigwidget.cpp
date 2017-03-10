#include "portconfigwidget.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include "XenaManager/src/treeView/ChassisViewWidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/port.h"
#include "XenaManager/src/Communicate/observer.h"
#include "XenaManager/src/inc/xt_utility.h"
#include <QLabel>
#include <QLineEdit>
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

enum E_TXMODE{
    NORMAL_MODE,
    STRICT_MODE,
    SEQUENTIAL_MODE
};

PortConfigWidget::PortConfigWidget(CXenaTester * xenaTester,ChassisViewWidget * chassisView,QWidget *parent)
	:BaseConfigWidget(parent),
	m_pXenaTester(xenaTester),
    m_pChassisViewWidget(chassisView),
    m_portObserver(new CPortObserver(xenaTester, this))
{
	initUI();
	// ±¾µØ
	connect(m_pHeadLengthComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
    connect(m_pTxModeComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
	connect(m_pKeepTimeEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pPortRateFraction,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pPortPacketRate,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pPortBitRate,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
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

            updateTxMode();

            connect(m_pChassisViewWidget,SIGNAL(updatePortTraffic(int,int)),this,SLOT(updateTraffic(int,int)));
        }
    }
}

void PortConfigWidget::initUI()
{
	m_trafficed = false;

	BaseConfigWidget::initUI();

    m_pHeaderLengthLabel = new QLabel(tr("HeaderLength:"));
	m_pKeepTimeLabel = new QLabel(tr("KeepTime:"));

	m_pHeadLengthComBox = new QComboBox;
    m_pHeadLengthComBox->setView(new QListView());
	m_pHeadLengthComBox->addItem("1024bytes");
	m_pHeadLengthComBox->addItem("512bytes");
	m_pHeadLengthComBox->addItem("256bytes");
	m_pHeadLengthComBox->addItem("128bytes");

	m_pStartBtn = new QPushButton(tr("Start"));
	m_pStopBtn = new QPushButton(tr("Stop"));

	m_pKeepTimeEdit = new QTimeEdit;
	m_pKeepTimeEdit->setEnabled(false);
    m_pKeepTimeEdit->setDisplayFormat("hh:mm:ss");

    m_pTxModeComBox = new QComboBox;
    m_pTxModeComBox->setView(new QListView());
    m_pTxModeComBox->addItem(tr("Normal"));
    m_pTxModeComBox->addItem(tr("Strict Uniforr"));
    m_pTxModeComBox->addItem(tr("Sequential"));

    m_pPortRateFraction = new QLineEdit;
    m_pPortRateFraction->setText("0.00");
    m_pPortPacketRate = new QLineEdit;
    m_pPortPacketRate->setText("0");
    m_pPortBitRate = new QLineEdit;
    m_pPortBitRate->setText("0.000");

    m_pPortRateFraction->setFixedWidth(90);
    m_pPortPacketRate->setFixedWidth(90);
    m_pPortBitRate->setFixedWidth(90);

    setMainLayout();
}

void PortConfigWidget::onCurrentIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    else
    {
        if (m_pHeadLengthComBox == sender())
        {
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

            m_pXenaTester->setPortMaxHeaderLength(m_currentModuleIndex,m_currentPortIndex,headLength);
        }
        else if (m_pTxModeComBox == sender())
        {
            switch(index)
            {
            case NORMAL_MODE:
                m_pPortRateFraction->setEnabled(false);
                m_pPortPacketRate->setEnabled(false);
                m_pPortBitRate->setEnabled(false);

                m_pXenaTester->setPortTxModeNormal(m_currentModuleIndex,m_currentPortIndex);
                break;
            case STRICT_MODE:
                m_pPortRateFraction->setEnabled(false);
                m_pPortPacketRate->setEnabled(false);
                m_pPortBitRate->setEnabled(false);

                m_pXenaTester->setPortTxModeStrict(m_currentModuleIndex,m_currentPortIndex);
                break;
            case SEQUENTIAL_MODE:
                m_pPortRateFraction->setEnabled(true);
                m_pPortPacketRate->setEnabled(true);
                m_pPortBitRate->setEnabled(true);

                m_pXenaTester->setPortTxModeSequential(m_currentModuleIndex,m_currentPortIndex);

                updatePortRateFracetion();
                updatePortPacketRate();
                updatePortBitRate();
                break;
            default:
                break;
            }
        }
    }
}

void PortConfigWidget::onEditingFinished()
{
    if (m_pKeepTimeEdit == sender())
    {
        QTime time = m_pKeepTimeEdit->time();
        int millisecond = time.hour() * 3600 * 1000 + time.minute() * 60 * 1000 + time.second() * 1000;

        m_pXenaTester->setPortTimeLimit(m_currentModuleIndex,m_currentPortIndex,millisecond);
    }
    else if (m_pPortRateFraction == sender())
    {
        if (SEQUENTIAL_MODE == m_pTxModeComBox->currentIndex())
        {
            float fraction = m_pPortRateFraction->text().toFloat();
            m_pXenaTester->setPortRateFraction(m_currentModuleIndex,m_currentPortIndex,fraction);
        }
    }
    else if (m_pPortPacketRate == sender())
    {
        if (SEQUENTIAL_MODE == m_pTxModeComBox->currentIndex())
        {
            int packets = m_pPortPacketRate->text().toInt();
            m_pXenaTester->setPortPacketRate(m_currentModuleIndex,m_currentPortIndex,packets);
        }
    }
    else if (m_pPortBitRate == sender())
    {
        if (SEQUENTIAL_MODE == m_pTxModeComBox->currentIndex())
        {
            double bitRate = m_pPortBitRate->text().toDouble();
            m_pXenaTester->setPortBitRate(m_currentModuleIndex,m_currentPortIndex,bitRate);
        }
    }
}

void PortConfigWidget::onStartBtn()
{
    if (0 == m_pPort->streamCount()) {
        QMessageBox::information(this, tr("Information"), tr("No Add Stream"),QString(tr("OK")));
        return ;
    }

    bool addObserve = false;
    if (m_pPort->txTimeLimit() != 0) {
        addObserve = true;
    }
    else {
        for(int i = 0; i < m_pPort->streamCount(); i++) {
            CStream*stream = m_pPort->stream(i);
            if (stream->strategy() != CStream::STRATEGY_NONE) {
                addObserve = true;
                break;
            }
        }
    }

    m_pXenaTester->portStartSpeedTraffic(m_pPort);
    if (addObserve) {
        xt_msleep(500);
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

    QHBoxLayout *txModeLayout = new QHBoxLayout;
    txModeLayout->addWidget(new QLabel(tr("Tx Mode:")));
    txModeLayout->addWidget(m_pTxModeComBox);
    txModeLayout->addStretch();

    QHBoxLayout *fractionLayout = new QHBoxLayout;
    fractionLayout->addWidget(new QLabel(tr("Rate Fraction:")));
    fractionLayout->addWidget(m_pPortRateFraction);
    fractionLayout->addWidget(new QLabel("%"));
    fractionLayout->addStretch();

    QHBoxLayout *packetRateLayout = new QHBoxLayout;
    packetRateLayout->addWidget(new QLabel(tr("Packet Rate:")));
    packetRateLayout->addWidget(m_pPortPacketRate);
    packetRateLayout->addWidget(new QLabel(tr("packet/sec")));
    packetRateLayout->addStretch();

    QHBoxLayout *bitRateLayout = new QHBoxLayout;
    bitRateLayout->addWidget(new QLabel(tr("Bit Rate:")));
    bitRateLayout->addWidget(m_pPortBitRate);
    bitRateLayout->addWidget(new QLabel("Mbit/sec"));
    bitRateLayout->addStretch();

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
    mainLayout->addSpacing(20);
    mainLayout->addLayout(txModeLayout);
    mainLayout->addLayout(fractionLayout);
    mainLayout->addLayout(packetRateLayout);
    mainLayout->addLayout(bitRateLayout);
    mainLayout->addSpacing(30);
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
    m_pTxModeComBox->setEnabled(state);
    if (state)
    {
        if (SEQUENTIAL_MODE == m_pTxModeComBox->currentIndex())
        {
            m_pPortRateFraction->setEnabled(state);
            m_pPortPacketRate->setEnabled(state);
            m_pPortBitRate->setEnabled(state);
        }
    }
    else
    {
        m_pPortRateFraction->setEnabled(state);
        m_pPortPacketRate->setEnabled(state);
        m_pPortBitRate->setEnabled(state);
    }
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

void PortConfigWidget::updateTxMode()
{
    QString mode = m_pPort->txMode();
    if (mode.contains("Normal",Qt::CaseInsensitive))
    {
        m_pTxModeComBox->setCurrentIndex(NORMAL_MODE);
    }
    else if (mode.contains("Strict",Qt::CaseInsensitive))
    {
        m_pTxModeComBox->setCurrentIndex(STRICT_MODE);
    }
    else if (mode.contains("Sequential",Qt::CaseInsensitive))
    {
        m_pTxModeComBox->setCurrentIndex(SEQUENTIAL_MODE);

        updatePortRateFracetion();
        updatePortPacketRate();
        updatePortBitRate();
    }
}

void PortConfigWidget::updatePortRateFracetion()
{
    m_pPortRateFraction->setText(QString::number(m_pPort->rateFraction(),'f',2));
}

void PortConfigWidget::updatePortPacketRate()
{
    m_pPortPacketRate->setText(QString::number(m_pPort->packetRate()));
}

void PortConfigWidget::updatePortBitRate()
{
    m_pPortBitRate->setText(QString::number(m_pPort->bitRate(),'f',3));
}