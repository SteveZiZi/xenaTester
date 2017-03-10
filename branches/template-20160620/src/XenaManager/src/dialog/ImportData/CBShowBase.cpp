#include "CBShowBase.h"

#include <QtGui/QTabWidget>
#include <QDir>

#include <QDebug>

#include <QBoxLayout>



ICBShowBase::ICBShowBase(CB_TYPE type, QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
: QWidget(parent,flags)
, m_cbType(type)
{
}

ICBShowBase::~ICBShowBase()
{
}

void ICBShowBase::updateIedInfo(const IED_INFO* iedInfo)
{
    if (iedInfo) {
        if (m_iedInfo.name != iedInfo->name) {
            m_iedInfo.name = iedInfo->name;
            m_iedInfo.desc = iedInfo->desc;
            m_iedInfo.hasTxSmv = iedInfo->hasTxSmv;
            m_iedInfo.hasTxGoose = iedInfo->hasTxGoose;
            m_iedInfo.hasRxSmv = iedInfo->hasRxSmv;
            m_iedInfo.hasRxGoose = iedInfo->hasRxGoose;
            updateShow();
        }
    }
}