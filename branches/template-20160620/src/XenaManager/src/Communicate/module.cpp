#include "module.h"
#include "port.h"
#include "chassis.h"

#include <QDebug>

void CModule::dumpMsg()
{
    qDebug( "++++++++++++  module:%d  ++++++++++", itemOfChassis());

    qDebug() << "moduleName:" << m_name;

    Q_FOREACH(CPort* port_, m_ports)
        port_->dumpMsg();

    qDebug( "------------  module:%d  ----------", itemOfChassis());
}

CModule::CModule(CChassis* _chassis/* = NULL*/)
: m_chassis(_chassis)
, m_name("exception")
{

}

CModule::~CModule()
{
    Q_FOREACH(CPort* port_, m_ports)
        if (port_) delete port_;
}


int CModule::itemOfChassis() const
{
    if (m_chassis) 
        return m_chassis->findModule(this);

    return -1;
}

CPort* CModule::port(quint16 item) const
{
    if (item < m_ports.size()) {
        return m_ports.at(item);
    }
    return NULL;
}

int CModule::findPort(const CPort* port_) const
{
    if (!port_) {
        return -1;
    }

    int item = 0;
    Q_FOREACH(CPort* p, m_ports) {
        if (p == port_) {
            return item;
        }
        item++;
    }

    return -1;
}

bool CModule::appendPort(CPort* port_)
{
    if (!port_) {
        return false;
    }

    m_ports.push_back(port_);
    port_->setModule(this);
    return true;
}
