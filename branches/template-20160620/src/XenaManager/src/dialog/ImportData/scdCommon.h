#pragma once


struct IED_INFO {
    int item;                   //  IED position
    QString name;               //  IED name
    QString desc;               //  IED description
    bool hasTxSmv;              //  IED has TX SMV capacity
    bool hasRxSmv;              //  IED has RX SMV capacity
    bool hasTxGoose;            //  IED has TX GOOSE capacity
    bool hasRxGoose;            //  IED has RX GOOSE capacity

    IED_INFO(const QString & iedName = QString(), const QString & iedDesc = QString()) {
        item = -1;
        name = iedName;
        desc = iedDesc;
        hasTxGoose = false;
        hasTxSmv = false;
        hasRxGoose = false;
        hasRxSmv = false;
    }
};


enum CB_TYPE {
    CB_TX_SMV,
    CB_TX_GOOSE,
    CB_RX_SMV,
    CB_RX_GOOSE
};

//选中控制块的信息
struct SelectedCBInfo {
    QString iedName;        //  IED name
    CB_TYPE cbType;         //  CB Type
    QString cbMac;          //  CB MAC
    quint16 cbAppid;        //  CB Appid
};