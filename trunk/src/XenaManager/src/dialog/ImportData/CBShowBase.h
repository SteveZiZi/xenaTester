#pragma once

#include <QtGui/QWidget>
#include <QtGui/QStandardItemModel>
#include "scdCommon.h"


class QTabWidget;
class QTableWidget;
class QLabel;
class ICBShowBase : public QWidget
{
    Q_OBJECT
public:
    ICBShowBase(CB_TYPE type, QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~ICBShowBase();

    virtual void setupUI() = 0;

signals:
    void cbSelected(const SelectedCBInfo &cbInfo, const quint8* data, quint32 size, bool flag);

public slots:
    void updateIedInfo(const IED_INFO* iedInfo);

protected:
    virtual void updateShow() =0;


protected:
    CB_TYPE   m_cbType;
    IED_INFO  m_iedInfo;

};