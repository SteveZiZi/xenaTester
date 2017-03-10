#pragma once

#include <QtGui/QTabWidget>

#include "scdCommon.h"

class CCBTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    CCBTabWidget(QWidget *parent = 0);
    virtual ~CCBTabWidget();

signals:
    void cbSelected(const SelectedCBInfo &cbInfo, const quint8* data, quint32 size, bool flag);

public slots:
    void updateIedInfo(const IED_INFO * iedInfo);

private slots:
    void cbSelect(const SelectedCBInfo &cbInfo, const quint8* data, quint32 size, bool flag);
};