#include "streamconfigwidget.h"
#include "XenaManager/src/treeView/SegmentViewWidget.h"
#include "XenaManager/src/treeView/ChassisViewWidget.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/stream.h"
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QListView>
#include <QBoxLayout>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>

#define MAXPACKETRATE 4294967295
#define MAXLOOPCOUNT 100

StreamConfigWidget::StreamConfigWidget(CXenaTester *xenaTester,ChassisViewWidget * chassisView,QWidget *parent/* = 0*/)
	:BaseConfigWidget(parent),
	m_pXenaTester(xenaTester),
	m_pChassisViewWidget(chassisView),
    m_pTimer(NULL)
{
	initUI();
	// 更新报文长度
	connect(m_pSegmentViewWidget,SIGNAL(updateStreamLength(quint16)),this,SLOT(updateStreanLength(quint16)));
	// 更新segmenTreeview
    connect(this,SIGNAL(updateSegment()),m_pSegmentViewWidget,SLOT(updateSegment()));

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateWidget()));
}

StreamConfigWidget::~StreamConfigWidget()
{
    if(m_pTimer && m_pTimer->isActive()) {
        m_pTimer->stop();
        m_pTimer->deleteLater();
    }
}

void StreamConfigWidget::updateStreanLength(quint16 length)
{
	m_pPacketLengthLabel->setText(QString::number(length+4));  // 加4字节的CRC
}

void StreamConfigWidget::onTextChanged(const QString text)
{
    if (m_pStream->settingsType())
    {
        if (m_pRatePrecentLineEdit == sender())
        {
            changeLineEditFontColor(m_pRatePrecentLineEdit,text);
        }
        else if (m_pStartRateLineEdit == sender())
        {
            changeLineEditFontColor(m_pStartRateLineEdit,text);
        }
        else if (m_pEndRateLineEdit == sender())
        {
            changeLineEditFontColor(m_pEndRateLineEdit,text);
        }
        else if (m_pStepLineEdit == sender())
        {
            changeLineEditFontColor(m_pStepLineEdit,text);
        }
    }
}

void StreamConfigWidget::onEditingFinished()
{
    if (m_pRatePrecentLineEdit == sender())
    {
        float percent = m_pRatePrecentLineEdit->text().toFloat();
        if (!m_pStream->setTxPercent(percent))
        {
            m_pRatePrecentLineEdit->setText(QString::number(m_pStream->txPercent()));
            QString info = QString("%1%2 %3%4 %5").arg(QObject::tr("module"))
                                                  .arg(m_pStream->port()->module()->itemOfChassis())
                                                  .arg(QObject::tr("port"))
                                                  .arg(m_pStream->port()->itemOfModule())
                                                  .arg(QObject::tr("arrived max flow"));
            QMessageBox::information(0, QObject::tr("Error"), info);
        }
    }
    else if (m_pRatePacketLineEdit == sender())
    {
        int packetPerSec = m_pRatePacketLineEdit->text().toInt();
        if (!m_pStream->setTxFramesPerSecond(packetPerSec)) {
            m_pRatePacketLineEdit->setText(QString::number(m_pStream->txFramesPerSecond()));
            QString info = QString("%1%2 %3%4 %5").arg(QObject::tr("module"))
                                                  .arg(m_pStream->port()->module()->itemOfChassis())
                                                  .arg(QObject::tr("port"))
                                                  .arg(m_pStream->port()->itemOfModule())
                                                  .arg(QObject::tr("arrived max flow"));
            QMessageBox::information(0, QObject::tr("Error"), info);
        }
    }
    else if (m_pFramCountLineEdit == sender())
    {
        if (m_pStream->strategy() != CStream::STRATEGY_STEP) {
            int packetsLimit = m_pFramCountLineEdit->text().toInt();
            if (packetsLimit)
            {
                m_pStream->setStrategy(packetsLimit);
            }
            else
            {
                packetsLimit = -1;
                m_pStream->setStrategy();
            }
        }
        else {
            setStrategy();
        }
    }
    else
    {
        setStrategy();
    }
}

void StreamConfigWidget::onStateChanged(int state)
{
	if (m_pEnableCheckBox == sender())
	{
		if (Qt::Checked == state)
		{
			m_pEnableCheckBox->setText(tr("Start"));
			setControlEnable(true);

            m_pXenaTester->setStreamEnabled(m_currentModuleIndex,m_currentPortIndex,
                                                      m_currentStreamIndex,true);
		}
		else if (Qt::Unchecked == state)
		{
			m_pEnableCheckBox->setText(tr("Disable"));
			setControlEnable(false);

            m_pXenaTester->setStreamEnabled(m_currentModuleIndex,m_currentPortIndex,
                                                      m_currentStreamIndex,false);
		}
	}
}

void StreamConfigWidget::onToggle(bool on)
{
    QObject* senderObj = this->sender();

    if (senderObj == m_pStrategyGroupBox) {
        int packetsLimiet = m_pFramCountLineEdit->text().toInt();
        if (!on) {
            m_pRatePacketLineEdit->setEnabled(true);
            m_pRatePrecentLineEdit->setEnabled(true);
            if (!m_pPacketLimitGroupBox->isChecked()) {
                m_pStream->setStrategy();
            }
            else {
                m_pStream->setStrategy(packetsLimiet);
            }
        }
        else {
            m_pRatePacketLineEdit->setEnabled(false);
            m_pRatePrecentLineEdit->setEnabled(false);

            setStrategy();
        }
    }
    else if (senderObj == m_pPacketLimitGroupBox) {
        if (m_pStrategyGroupBox->isChecked()) {
            setStrategy();
        }
        else {
            if (on) {
                //  按帧数量发送
                int packetsLimiet = m_pFramCountLineEdit->text().toInt();
                if (packetsLimiet > 0) {
                    m_pStream->setStrategy(packetsLimiet);
                }
                else {
                    m_pStream->setStrategy();
                }
            }
            else {
                //  正常流程
                m_pStream->setStrategy();
            }
        }
    }

}

void StreamConfigWidget::onCurrentIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    switch (index)
    {
    case CStream::SETTINGS_PPS:
        packetsRateStyle();
        break;
    case CStream::SETTINGS_PERCENT:
        precentRateStyle();
        break;
    default:
        break;
    }
    m_pStream->setSettingsType(static_cast<CStream::SettingsType>(index));
    updateStrategy();
}

void StreamConfigWidget::updateWidget()
{
    CPort* port = m_pStream->port();
    if (!port->actived()) {
        m_pEnableCheckBox->setEnabled(true);
        setControlEnable(true);
        m_pTimer->stop();
    }
}

void StreamConfigWidget::updateConfigWidget()
{
    m_pStream = m_pXenaTester->chassis()->module(m_currentModuleIndex)->port(m_currentPortIndex)->stream(m_currentStreamIndex);

    disconnectLocalSlot();

    setTypeNameLabelContext(m_pStream->streamName());

    updateStreamEnableState();

    updateStreamPacketsLimit();

    updateStrategy();

    updateRateWay();

    connectLocalSlot();

    emit updateSegment();                                               // 更新segment

    QString portOwner = m_pStream->port()->reservedBy();
    if (portOwner == QString::fromStdString(LoginSettings::getUserName())) {
        if (m_pStream->port()->actived()) {
            m_pEnableCheckBox->setEnabled(false);
            setControlEnable(false);
            if (!m_pTimer->isActive()) {
                m_pTimer->start(500);
            }
        }
        else {
            m_pEnableCheckBox->setEnabled(true);
            setControlEnable(true);
            if (m_pTimer->isActive()) {
                m_pTimer->stop();
            }
        }
    }
    else {
        if (m_pTimer->isActive()) {
            m_pTimer->stop();
        }
        m_pEnableCheckBox->setEnabled(false);
        setControlEnable(false);
    }
}

void StreamConfigWidget::initUI()
{
    BaseConfigWidget::initUI();

    // 初始化正则表达式
    m_pRatePacketReg = new QIntValidator(0,MAXPACKETRATE,this);
    m_pRatePrecentReg = new QDoubleValidator(0.000,100.000,3,this);

    m_pFrame = new QFrame;
    m_pFrame->setMaximumHeight(MAXWIDGETHEIGHT);
    m_pSegmentViewWidget = new SegmentViewWidget(m_pXenaTester);

    m_pEnableCheckBox = new QCheckBox(tr("Enable"));

    //  报文长度
    m_pLengthLabel = new QLabel(tr("PacketLength:"));
    m_pPacketLengthLabel = new QLabel;

    //  速率方式
    m_pSettingTypeLabel = new QLabel(tr("RateWay:"));
    m_pSettingTypeComBox = new QComboBox;
    m_pSettingTypeComBox->setView(new QListView());
    m_pSettingTypeComBox->addItem(tr("PksPerSec"));
    m_pSettingTypeComBox->addItem(tr("Precent"));

    //  发送速率
    m_pRateLabel = new QLabel(tr("SendRate:"));
    m_pRatePrecentUnitLabel = new QLabel(tr("%"));
    m_pRatePacketUnitLabel = new QLabel(tr("pcs/sec"));
    m_pRatePrecentLineEdit = new QLineEdit;
    QDoubleValidator *ratePrecentReg = new QDoubleValidator(0.000,100.000,3,this);
    ratePrecentReg->setNotation(QDoubleValidator::StandardNotation);	// 
    m_pRatePrecentLineEdit->setValidator(ratePrecentReg);
    m_pRatePrecentLineEdit->setMaximumWidth(LINEEDITWIDTH / 2);

    m_pRatePacketLineEdit = new QLineEdit;
    m_pRatePacketLineEdit->setMaximumWidth(LINEEDITWIDTH / 2);
    m_pRatePacketLineEdit->setValidator(new QIntValidator(0,MAXPACKETRATE,this));

    //  帧数量
    m_pPacketLimitGroupBox = new QGroupBox(tr("Send as Packets"));       //按帧发送
    m_pPacketLimitGroupBox->setMaximumWidth(MAXWIDGETHEIGHT);
    m_pPacketLimitGroupBox->setCheckable(true);
    m_pFrameCountLabel = new QLabel(tr("PacketsNum:  "));
    m_pFramCountLineEdit = new QLineEdit;
    m_pFramCountLineEdit->setValidator(new QIntValidator(1,MAXPACKETRATE,this));
    m_pFramCountLineEdit->setMaximumWidth(LINEEDITWIDTH / 2);
    
    //  步进策略
    m_pStrategyGroupBox = new QGroupBox(tr("StepStyle"));
    m_pStrategyGroupBox->setMaximumWidth(MAXWIDGETHEIGHT);
    m_pStrategyGroupBox->setCheckable(true);

    m_pStartRateLabel = new QLabel(tr("StartRate:"));
    m_pEndRateLabel = new QLabel(tr("EndRate:"));
    m_pStepLabel = new QLabel(tr("Step"));
    m_pLoopCountLabel = new QLabel(tr("LoopTimes:"));
    m_pStartRateUnitLabel = new QLabel(tr("pcs/sec"));
    m_pEndRateUnitLabel = new QLabel(tr("pcs/sec"));
    m_pStepUnitLabel = new QLabel(tr("pcs/sec"));

    m_pStartRateLineEdit = new QLineEdit;
    m_pStartRateLineEdit->setMaximumWidth(LINEEDITWIDTH);
    m_pEndRateLineEdit = new QLineEdit;
    m_pEndRateLineEdit->setMaximumWidth(LINEEDITWIDTH);
    m_pStepLineEdit = new QLineEdit;
    m_pStepLineEdit->setMaximumWidth(LINEEDITWIDTH);
    m_pLoopCountLineEdit = new QLineEdit;
    m_pLoopCountLineEdit->setValidator(new QIntValidator(0,MAXLOOPCOUNT,this));
    m_pLoopCountLineEdit->setMaximumWidth(LINEEDITWIDTH);


    setMainLayout();
}

void StreamConfigWidget::updateStreamPacketsLimit()
{
    m_pFramCountLineEdit->setText(QString::number(m_pStream->packetLimit()));
}

void StreamConfigWidget::updateStreamEnableState()
{
    if (m_pStream->isEnableTx())
    {
        m_pEnableCheckBox->setChecked(true);
        m_pEnableCheckBox->setText(tr("Start"));
    }
    else
    {
        m_pEnableCheckBox->setChecked(false);
        m_pEnableCheckBox->setText(tr("Disable"));
    }
}

void StreamConfigWidget::updateTxFramesPerSecond()
{
    int TxFramesPerSecond = m_pStream->txFramesPerSecond();
    m_pRatePacketLineEdit->setText(QString::number(TxFramesPerSecond));
}

void StreamConfigWidget::updateTxPrecent()
{
    float precent = m_pStream->txPercent();
    m_pRatePrecentLineEdit->setText(QString::number(precent,'f',3));
}

void StreamConfigWidget::updateStrategy()
{
    m_pLoopCountLineEdit->setText(QString::number(m_pStream->loopCnt()));
    updateStreamPacketsLimit();
    // 帧每秒
    if (CStream::SETTINGS_PPS == m_pStream->settingsType())
    {
        packetsRateStyle();
        m_pStartRateLineEdit->setText(QString::number(m_pStream->startFramesPerSec()));
        m_pEndRateLineEdit->setText(QString::number(m_pStream->endFramesPerSec()));
        m_pStepLineEdit->setText(QString::number(m_pStream->stepFramesPerSec()));

        updateTxFramesPerSecond();
    }
    // 百分比
    else if (CStream::SETTINGS_PERCENT == m_pStream->settingsType())
    {
        precentRateStyle();
        m_pStartRateLineEdit->setText(QString::number(m_pStream->startPercent(),'f',3));
        m_pEndRateLineEdit->setText(QString::number(m_pStream->endPercent(),'f',3));
        m_pStepLineEdit->setText(QString::number(m_pStream->stepPercent(),'f',3));

        updateTxPrecent();
    }

    if (CStream::STRATEGY_NONE == m_pStream->strategy())
    {
        m_pStrategyGroupBox->setChecked(false);
        m_pPacketLimitGroupBox->setChecked(false);
    }
    else if (CStream::STRATEGY_FRAMES == m_pStream->strategy())
    {
        m_pStrategyGroupBox->setChecked(false);
        m_pPacketLimitGroupBox->setChecked(true);
    }
    else if (CStream::STRATEGY_STEP == m_pStream->strategy())
    {
        m_pStrategyGroupBox->setChecked(true);
    }
}

void StreamConfigWidget::updateRateWay()
{
    switch(m_pStream->settingsType())
    {
    case CStream::SETTINGS_PPS:
        m_pSettingTypeComBox->setCurrentIndex(0);
        break;
    case CStream::SETTINGS_PERCENT:
        m_pSettingTypeComBox->setCurrentIndex(1);
        break;
    default:
        break;
    }
}

void StreamConfigWidget::setStrategy()
{
    int   packetsLimit = 0;
    if (m_pPacketLimitGroupBox->isChecked()) {
        packetsLimit = m_pFramCountLineEdit->text().toInt();
    }
    else {
        m_pFramCountLineEdit->setText("0");
    }

    if (m_pStream->settingsType() == CStream::SETTINGS_PERCENT)
    {
        // 百分比
        float start = m_pStartRateLineEdit->text().toFloat();
        float end = m_pEndRateLineEdit->text().toFloat();
        float step = m_pStepLineEdit->text().toFloat();
        int   loopCnt = m_pLoopCountLineEdit->text().toInt();
        
        if (!m_pStream->setTxPercent(start))
        {
            m_pStartRateLineEdit->setText(QString::number(m_pStream->txPercent()));
            QString info = QString("%1%2 %3%4 %5").arg(QObject::tr("module"))
                                                  .arg(m_pStream->port()->module()->itemOfChassis())
                                                  .arg(QObject::tr("port"))
                                                  .arg(m_pStream->port()->itemOfModule())
                                                  .arg(QObject::tr("arrived max flow"));
            QMessageBox::information(0, QObject::tr("Error"), info);
        }
        else {
            m_pStream->setStrategy(packetsLimit,start,end,step,loopCnt);
            m_pRatePrecentLineEdit->setText(m_pStartRateLineEdit->text());
        }
    }
    else
    {
        // 帧每秒
        int start = m_pStartRateLineEdit->text().toInt();
        int end = m_pEndRateLineEdit->text().toInt();
        int step = m_pStepLineEdit->text().toInt();
        int loopCnt = m_pLoopCountLineEdit->text().toInt();
        if (!m_pStream->setTxFramesPerSecond(start)) {
            m_pStartRateLineEdit->setText(QString::number(m_pStream->txFramesPerSecond()));
            QString info = QString("%1%2 %3%4 %5").arg(QObject::tr("module"))
                                                  .arg(m_pStream->port()->module()->itemOfChassis())
                                                  .arg(QObject::tr("port"))
                                                  .arg(m_pStream->port()->itemOfModule())
                                                  .arg(QObject::tr("arrived max flow"));
            QMessageBox::information(0, QObject::tr("Error"), info);
        }
        else {
            m_pStream->setStrategy(packetsLimit,start,end,step,loopCnt);
            m_pRatePacketLineEdit->setText(m_pStartRateLineEdit->text());
        }
    }
}

void StreamConfigWidget::setMainLayout()
{
    //  caption
    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(m_pNameLabel);
    nameLayout->addWidget(m_pTypeNameLabel); 
    nameLayout->addWidget(m_pEnableCheckBox);
    nameLayout->addStretch();

    //  报文长度
    QHBoxLayout *packetLengthLayout = new QHBoxLayout;
    packetLengthLayout->addWidget(m_pLengthLabel);
    packetLengthLayout->addWidget(m_pPacketLengthLabel);
    packetLengthLayout->addWidget(new QLabel(tr("Byte")));
    packetLengthLayout->addStretch();

    //  速率方式
    QHBoxLayout *rateWayLayout = new QHBoxLayout;
    rateWayLayout->addWidget(m_pSettingTypeLabel);
    rateWayLayout->addWidget(m_pSettingTypeComBox);
    rateWayLayout->addStretch();

    //  发送速率
    QHBoxLayout *rateLayout = new QHBoxLayout;
    rateLayout->addWidget(m_pRateLabel);
    rateLayout->addWidget(m_pRatePrecentLineEdit);
    rateLayout->addWidget(m_pRatePrecentUnitLabel);
    rateLayout->addWidget(m_pRatePacketLineEdit);
    rateLayout->addWidget(m_pRatePacketUnitLabel);
    rateLayout->addStretch();

    //  帧数量
    QHBoxLayout *packetLayout = new QHBoxLayout;
    packetLayout->addWidget(m_pFrameCountLabel);
    packetLayout->addWidget(m_pFramCountLineEdit);
    packetLayout->addWidget(new QLabel(tr("Packets")));
    packetLayout->addStretch();
    m_pPacketLimitGroupBox->setLayout(packetLayout);

    //  步进策略
	QHBoxLayout *startRateLayout = new QHBoxLayout;
	startRateLayout->addWidget(m_pStartRateLabel);
	startRateLayout->addWidget(m_pStartRateLineEdit);
	startRateLayout->addWidget(m_pStartRateUnitLabel);

	QHBoxLayout *endRateLayout = new QHBoxLayout;
	endRateLayout->addWidget(m_pEndRateLabel);
	endRateLayout->addWidget(m_pEndRateLineEdit);
	endRateLayout->addWidget(m_pEndRateUnitLabel);

	QHBoxLayout *stepLayout = new QHBoxLayout;
	stepLayout->addWidget(m_pStepLabel);
	stepLayout->addWidget(m_pStepLineEdit);
	stepLayout->addWidget(m_pStepUnitLabel);

	QHBoxLayout *loopCountLayout = new QHBoxLayout;
	loopCountLayout->addWidget(m_pLoopCountLabel);
	loopCountLayout->addWidget(m_pLoopCountLineEdit);
	loopCountLayout->addWidget(new QLabel(tr("Times")));

	QVBoxLayout *groupBoxLayout = new QVBoxLayout;
	groupBoxLayout->addLayout(startRateLayout);
	groupBoxLayout->addLayout(endRateLayout);
	groupBoxLayout->addLayout(stepLayout);
	groupBoxLayout->addLayout(loopCountLayout);
	m_pStrategyGroupBox->setLayout(groupBoxLayout);


	QVBoxLayout *leftLayout = new QVBoxLayout;
	leftLayout->addLayout(nameLayout);
	leftLayout->addLayout(packetLengthLayout);
    leftLayout->addLayout(rateWayLayout);
    leftLayout->addLayout(rateLayout);
	leftLayout->addWidget(m_pPacketLimitGroupBox);

	QVBoxLayout *rightLayout = new QVBoxLayout;
	rightLayout->addWidget(m_pStrategyGroupBox);

	QHBoxLayout *middleLayout = new QHBoxLayout;
	middleLayout->addLayout(leftLayout);
	middleLayout->addSpacing(GAPBETWEENLAYOUT);
	middleLayout->addLayout(rightLayout);
	middleLayout->addStretch();

	QVBoxLayout *frameLayout = new QVBoxLayout;
  	frameLayout->addLayout(middleLayout);
	m_pFrame->setLayout(frameLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pFrame);
	mainLayout->addWidget(m_pSegmentViewWidget);
	mainLayout->setMargin(1);

	this->setLayout(mainLayout);
}

void StreamConfigWidget::changeLineEditFontColor(QLineEdit *lineEdit,const QString text)
{
	if (text.toDouble() > 100.0)
	{
		lineEdit->setStyleSheet("color:red");							//文本颜色  
	}
	else
	{
		lineEdit->setStyleSheet("color:black");							//文本颜色 
	}
	//lineEdit->setText(text);
}

void StreamConfigWidget::setControlEnable(bool state)
{
    if (m_pStrategyGroupBox->isChecked()) {
        m_pRatePrecentLineEdit->setEnabled(false);
        m_pRatePacketLineEdit->setEnabled(false);
    }
    else {
        m_pRatePrecentLineEdit->setEnabled(state);
        m_pRatePacketLineEdit->setEnabled(state);
    }

    m_pSegmentViewWidget->setEnabled(state);

	m_pStrategyGroupBox->setEnabled(state);
	m_pPacketLimitGroupBox->setEnabled(state);
    m_pSettingTypeComBox->setEnabled(state);
}

void StreamConfigWidget::packetsRateStyle()
{
    m_pRatePrecentLineEdit->hide();
    m_pRatePrecentUnitLabel->hide();

    m_pRatePacketLineEdit->show();
    m_pRatePacketUnitLabel->show();

    m_pStartRateLineEdit->setValidator(m_pRatePacketReg);
    m_pEndRateLineEdit->setValidator(m_pRatePacketReg);
    m_pStepLineEdit->setValidator(m_pRatePacketReg);

    m_pStartRateUnitLabel->setText(tr("pcs/sec"));
    m_pEndRateUnitLabel->setText(tr("pcs/sec"));
    m_pStepUnitLabel->setText(tr("pcs/sec"));
}

void StreamConfigWidget::precentRateStyle()
{
    m_pRatePrecentLineEdit->show();
    m_pRatePrecentUnitLabel->show();

    m_pRatePacketLineEdit->hide();
    m_pRatePacketUnitLabel->hide();

    m_pStartRateLineEdit->setValidator(m_pRatePrecentReg);
    m_pEndRateLineEdit->setValidator(m_pRatePrecentReg);
    m_pStepLineEdit->setValidator(m_pRatePrecentReg);

    m_pStartRateUnitLabel->setText(tr("%"));
    m_pEndRateUnitLabel->setText(tr("%"));
    m_pStepUnitLabel->setText(tr("%"));
}


void StreamConfigWidget::connectLocalSlot()
{
    // 本地控件自身相关
    connect(m_pRatePrecentLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));
    connect(m_pStartRateLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));
    connect(m_pEndRateLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));
    connect(m_pStepLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));

    connect(m_pRatePrecentLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pRatePacketLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pFramCountLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pStartRateLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pEndRateLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pStepLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    connect(m_pLoopCountLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));

    connect(m_pEnableCheckBox,SIGNAL(stateChanged(int)),this,SLOT(onStateChanged(int)));
    connect(m_pStrategyGroupBox,SIGNAL(toggled(bool)),this,SLOT(onToggle(bool)));
    connect(m_pPacketLimitGroupBox,SIGNAL(toggled(bool)),this,SLOT(onToggle(bool)));
    connect(m_pSettingTypeComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
}

void StreamConfigWidget::disconnectLocalSlot()
{
    // 本地控件自身相关
    disconnect(m_pRatePrecentLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));
    disconnect(m_pStartRateLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));
    disconnect(m_pEndRateLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));
    disconnect(m_pStepLineEdit,SIGNAL(textChanged(const QString)),this,SLOT(onTextChanged(const QString)));

    disconnect(m_pRatePrecentLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    disconnect(m_pRatePacketLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    disconnect(m_pFramCountLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    disconnect(m_pStartRateLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    disconnect(m_pEndRateLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    disconnect(m_pStepLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));
    disconnect(m_pLoopCountLineEdit,SIGNAL(editingFinished()),this,SLOT(onEditingFinished()));

    disconnect(m_pEnableCheckBox,SIGNAL(stateChanged(int)),this,SLOT(onStateChanged(int)));
    disconnect(m_pPacketLimitGroupBox,SIGNAL(toggled(bool)),this,SLOT(onToggle(bool)));
    disconnect(m_pStrategyGroupBox,SIGNAL(toggled(bool)),this,SLOT(onToggle(bool)));
    disconnect(m_pSettingTypeComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
}