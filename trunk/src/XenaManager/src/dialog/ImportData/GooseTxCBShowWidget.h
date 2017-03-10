#pragma once


#include "TxCBShowWidget.h"
#include "sclManager/include/sclManager.h"

class CScdAnalysis;
class CGooseTxCBShowWidget : public ITxCBShowWidget
{
    Q_OBJECT
public:
    CGooseTxCBShowWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~CGooseTxCBShowWidget();

    enum CB_COLUMN {
        CB_CHECKED,
        CB_COL_APPID,
        CB_COL_MAC_ADDR,
        CB_COL_CHNL_NUM,
        CB_COL_DATA_DESC,
        CB_COL_DATASET,
        CB_COL_GOID,
        CB_COL_GOCB_REF,
        CB_COL_VLAN_ID,
        CB_COL_VLAN_PRIO,
        CB_COL_CONF_REF,

        MAX_CB_COL_NUM
    };
    enum CHNL_COLUMN {
        CHNL_COL_ITEM,
        CHNL_COL_TYPE,
        CHNL_COL_DESC_LN,
        CHNL_COL_DESC_DO,
        CHNL_COL_DESC_DU,
        CHNL_COL_PATH,

        MAX_CHNL_COL_NUM
    };

protected:
    //override from ICBShowBase
    virtual void updateShow();
    //override from ITxCBShowWidget
    virtual int exportFrame(quint16 &appid, QString& mac);
    virtual QTableWidget* createCBTableWidget();
    virtual QTableWidget* createChnlTableWidget();


private slots:
    void cbChanged();
private:
    void addGseCB(const SCDGOOSEANALYSIS_IEDGOOSE_INFO& gesCB);

private:
    CScdAnalysis* m_analysis;
};