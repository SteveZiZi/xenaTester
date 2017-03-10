#ifndef STREAMCOMFIGWIDGET_H
#define STREAMCOMFIGWIDGET_H

#include <QWidget>
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"

enum E_PORT_SPEED_TYPE{
	F10M = 10,
	F100M = 100,
	F1G = 1000,
	F10G = 10000,
	F40G = 40000,
	F100G = 100000
};

class CXenaTester;
class ChassisViewWidget;
class SegmentViewWidget;
class CStream;
class QLabel;
class QPushButton;
class QLineEdit;
class QFrame;
class QCheckBox;
class QGroupBox;
class QComboBox;
class StreamConfigWidget : public BaseConfigWidget
{
	Q_OBJECT

public:
	StreamConfigWidget(CXenaTester *xenaTester,ChassisViewWidget * chassisView,QWidget *parent = 0);
	virtual~StreamConfigWidget();
	//override from BaseConfigWidget
	virtual void updateConfigWidget();


    int currentModuleIndex() {return m_currentModuleIndex;}
    int currentPortIndex() {return m_currentPortIndex;}
    int currentStreamIndex() {return m_currentStreamIndex;}
    int currentStreamModifierCnt();
    int currentStreamMaxModifierCnt();
    CStream * currentStream() {return m_pStream;}

signals:
    void updateSegment();

protected:
	//override from BaseConfigWidget
	virtual void initUI();

private slots:
	void updateStreanLength(quint16 length);							// 更新流配置界面中的数据长度

	void onTextChanged(const QString text);								// 超限后增加颜色显示
	void onEditingFinished();
	void onStateChanged(int state);
	void onToggle(bool on);
    void onCurrentIndexChanged(int index);                              // 速率方式

private:
    void updateStreamPacketsLimit();                                    // 更新流的帧数限制
    void updateStreamEnableState();                                     // 更新流的使能状态
    void updateTxFramesPerSecond();                                     // 更新帧每秒
    void updateTxPrecent();                                             // 更新帧百分比
    void updateStrategy();                                              // 更新策略内容

    void setStrategy();                                                 // 设置帧策略

    void setMainLayout();
    void changeLineEditFontColor(QLineEdit *lineEdit,const QString text);// 针对百分比有颜色变换显示
    void setControlEnable(bool state);
    void packetsRateStyle();
    void precentRateStyle();

    void connectLocalSlot();
    void disconnectLocalSlot();

#if 0
    void updatePortSpeed();                                             // 更新流对应的端口的速率
    void calcRatePrecent();												// 根据帧每秒计算速率(百分比)
    void calcRatePacket();												// 根据百分比计算帧每秒
#endif

private:
	CXenaTester			*m_pXenaTester;
	ChassisViewWidget	*m_pChassisViewWidget;
    CStream             *m_pStream;

	SegmentViewWidget	*m_pSegmentViewWidget;

	QCheckBox			*m_pEnableCheckBox;
	

	QGroupBox			*m_pStrategyGroupBox;

	QLabel				*m_pRateLabel;
	QLabel				*m_pRatePrecentUnitLabel;
	QLabel				*m_pRatePacketUnitLabel;
    
    QLabel			    *m_pFrameCountLabel;
	QLabel				*m_pLengthLabel;
	QLabel				*m_pPacketLengthLabel;
	QLabel				*m_pStartRateLabel;
	QLabel				*m_pEndRateLabel;
	QLabel				*m_pStepLabel;
	QLabel				*m_pLoopCountLabel;
    QLabel              *m_pStartRateUnitLabel;
    QLabel              *m_pEndRateUnitLabel;
    QLabel              *m_pStepUnitLabel;
    QLabel              *m_pSettingTypeLabel;

	QLineEdit			*m_pFramCountLineEdit;
	QLineEdit			*m_pRatePrecentLineEdit;
	QLineEdit			*m_pRatePacketLineEdit;

	QLineEdit			*m_pStartRateLineEdit;
	QLineEdit			*m_pEndRateLineEdit;
	QLineEdit			*m_pStepLineEdit;
	QLineEdit			*m_pLoopCountLineEdit;

    QComboBox           *m_pSettingTypeComBox;

	QFrame				*m_pFrame;

	quint8				*m_pFrames;

    /*E_PORT_SPEED_TYPE   m_portSpeedType;*/
};

#endif // STREAMCOMFIGWIDGET_H
