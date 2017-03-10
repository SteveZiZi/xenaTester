#include "CBTabWidget.h"

#include <QtGui/QTabWidget>
#include <QDir>

#include <QDebug>

#include <QBoxLayout>

#include "SmvTxCBShowWidget.h"
#include "GooseTxCBShowWidget.h"



CCBTabWidget::CCBTabWidget(QWidget *parent/* = 0*/)
: QTabWidget(parent)
{
    ICBShowBase* smvShow = new CSmvTxCBShowWidget();
    addTab(smvShow, CCBTabWidget::tr("SMV Send"));
    smvShow->setupUI();
    ICBShowBase* gseShow = new CGooseTxCBShowWidget();
    addTab(gseShow, CCBTabWidget::tr("GOOSE Send"));
    gseShow->setupUI();

    connect(smvShow, SIGNAL(cbSelected(const SelectedCBInfo &, const quint8*, quint32, bool)), this, SLOT(cbSelect(const SelectedCBInfo &, const quint8*, quint32, bool)));
    connect(gseShow, SIGNAL(cbSelected(const SelectedCBInfo &, const quint8*, quint32, bool)), this, SLOT(cbSelect(const SelectedCBInfo &, const quint8*, quint32, bool)));
}

CCBTabWidget::~CCBTabWidget()
{
}

void CCBTabWidget::updateIedInfo(const IED_INFO * iedInfo)
{
    for(int i = 0; i < this->count(); i++) {
        ICBShowBase* cbShow = dynamic_cast<ICBShowBase*>(this->widget(i));
        if (cbShow)
            cbShow->updateIedInfo(iedInfo);
    }
    if (iedInfo) {
        this->setTabEnabled(CB_TX_SMV, iedInfo->hasTxSmv);
        this->setTabEnabled(CB_TX_GOOSE, iedInfo->hasTxGoose);
    }
    else {
        for(int i = 0; i < this->count(); i++) {
            this->setTabEnabled(i, false);
        }
    }
}


void CCBTabWidget::cbSelect(const SelectedCBInfo &cbInfo, const quint8* data, quint32 size, bool flag)
{
    emit cbSelected(cbInfo, data, size, flag);
}