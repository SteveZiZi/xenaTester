#pragma once


#include "CBShowBase.h"

class QTabWidget;
class QTableWidget;
class QLabel;
class QAction;
class ITxCBShowWidget : public ICBShowBase
{
    Q_OBJECT
public:
    ITxCBShowWidget(CB_TYPE type, QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~ITxCBShowWidget();

protected:
    //override from ICBShowBase
    virtual void updateShow() =0;
    virtual void setupUI();
    //own
    /*
     *  @Func:  exportFrame   ����ѡ�еĿ��ƿ鱨��
     *  
     *  @Param[out]:
     *      appid:            �������ƿ��APPID��
     *      mac:              �������ƿ��MAC��ַ
     *  
     *  @Return:
     *      >0:               �������ƿ鱨�ĵĳ���
     */
    virtual int exportFrame(quint16 &appid, QString& mac) = 0;
    virtual QTableWidget* createCBTableWidget() = 0;
    virtual QTableWidget* createChnlTableWidget() = 0;

private slots:
    void exprotCB();
    void onCustomContextMenuRequested(const QPoint &point);

protected:
    QLabel* m_captionLabel;
    QTableWidget *m_tableWidgetCB;
    QTableWidget *m_tableWidgetChnl;

    QAction*    m_selectionAction;
    quint8*     m_frameBuf;
};