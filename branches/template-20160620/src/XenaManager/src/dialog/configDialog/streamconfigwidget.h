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
	void updateStreanLength(quint16 length);							// ���������ý����е����ݳ���

	void onTextChanged(const QString text);								// ���޺�������ɫ��ʾ
	void onEditingFinished();
	void onStateChanged(int state);
	void onToggle(bool on);
    void onCurrentIndexChanged(int index);                              // ���ʷ�ʽ

private:
    void updateStreamPacketsLimit();                                    // ��������֡������
    void updateStreamEnableState();                                     // ��������ʹ��״̬
    void updateTxFramesPerSecond();                                     // ����֡ÿ��
    void updateTxPrecent();                                             // ����֡�ٷֱ�
    void updateStrategy();                                              // ���²�������

    void setStrategy();                                                 // ����֡����

    void setMainLayout();
    void changeLineEditFontColor(QLineEdit *lineEdit,const QString text);// ��԰ٷֱ�����ɫ�任��ʾ
    void setControlEnable(bool state);
    void packetsRateStyle();
    void precentRateStyle();

    void connectLocalSlot();
    void disconnectLocalSlot();

#if 0
    void updatePortSpeed();                                             // ��������Ӧ�Ķ˿ڵ�����
    void calcRatePrecent();												// ����֡ÿ���������(�ٷֱ�)
    void calcRatePacket();												// ���ݰٷֱȼ���֡ÿ��
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
