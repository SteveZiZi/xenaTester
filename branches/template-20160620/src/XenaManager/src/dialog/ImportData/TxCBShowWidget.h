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
     *  @Func:  exportFrame   导出选中的控制块报文
     *  
     *  @Param[out]:
     *      appid:            导出控制块的APPID号
     *      mac:              导出控制块的MAC地址
     *  
     *  @Return:
     *      >0:               导出控制块报文的长度
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