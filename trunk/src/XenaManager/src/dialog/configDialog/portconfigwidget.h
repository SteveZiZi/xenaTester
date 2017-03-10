#ifndef PORTCONFIGWIDGET_H
#define PORTCONFIGWIDGET_H

#include <QWidget>
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"

class CXenaTester;
class ChassisViewWidget;
class CPort;
class CPortObserver;
class QPushButton;
class QComboBox;
class QTimeEdit;
class QTimer;
class QLineEdit;
class PortConfigWidget : public BaseConfigWidget
{
	Q_OBJECT

public:
	PortConfigWidget(CXenaTester * xenaTester,ChassisViewWidget * chassisView,QWidget *parent = 0);
	virtual ~PortConfigWidget();
	//override from BaseConfigWidget
	virtual void updateConfigWidget();

protected:
	//override from BaseConfigWidget
	virtual void initUI();

private slots:

	void onCurrentIndexChanged(int index);
	void onEditingFinished();
	void onStartBtn();
	void onStopBtn();

    void updateTraffic(int moduleIndex,int portIndex);

private:
    void setMainLayout();
    void setControlEnable(bool state);
    void updateHeadLength();                                            // 更新头长度
    void updateTrafficStatus();                                         // 更新发送状态
    void updateTxTimeLimit();                                           // 更新保持时间

    // 更新端口速率相关
    void updateTxMode();
    void updatePortRateFracetion();
    void updatePortPacketRate();
    void updatePortBitRate();

private:
    CXenaTester			*m_pXenaTester;
    ChassisViewWidget	*m_pChassisViewWidget;
    CPort               *m_pPort;

	QLabel				*m_pHeaderLengthLabel;
	QLabel              *m_pKeepTimeLabel;
	QComboBox			*m_pHeadLengthComBox;
	QPushButton			*m_pStartBtn;
	QPushButton			*m_pStopBtn;
	QTimeEdit			*m_pKeepTimeEdit;

    QComboBox           *m_pTxModeComBox;
    QLineEdit           *m_pPortRateFraction;
    QLineEdit           *m_pPortPacketRate;
    QLineEdit           *m_pPortBitRate;

	bool				m_trafficed;									// 端口发送状态 true 表示发送中 false表示停止

    CPortObserver       *m_portObserver;
};

#endif // PORTCONFIGWIDGET_H
