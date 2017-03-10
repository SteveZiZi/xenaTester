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
, m_name("empty Module")
, m_bBadModule(true)
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


void CModule::setName(const QString &name_)
{
    if (m_chassis) {
        m_name = QString("Module%1 %2").arg(itemOfChassis()).arg(name_);
    }
    else {
        m_name = name_;
    }
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

bool CModule::toXml(const CModule *module_, QDomDocument &doc, QDomElement &moduletDom)
{
    if (!module_) {
        return false;
    }

    QDomElement child = doc.createElement("isBadModule");
    child.setAttribute(QString("val"), module_->m_bBadModule);
    moduletDom.appendChild(child);

    child = doc.createElement("Reserved");
    child.setAttribute(QString("val"), module_->m_bReserved);
    child.setAttribute(QString("name"), module_->m_reservedBy);
    moduletDom.appendChild(child);

    child = doc.createElement("ports");
    Q_FOREACH(CPort* port_, module_->m_ports)
    {
        QDomElement portDom = doc.createElement("port");
        portDom.setAttribute(QString("item"), port_->itemOfModule());
        if (!CPort::toXml(port_, doc, portDom)) {
            return false;
        }
        child.appendChild(portDom);
    }
    moduletDom.appendChild(child);

    return true;
}

bool CModule::fromXml(QDomElement &moduletDom, CModule &module_)
{
    try
    {
        QDomElement ele = moduletDom.firstChildElement(QString("isBadPort"));
        module_.m_bBadModule = ele.attribute(QString("val"), QString("0")).toInt() ? true : false;

        ele = moduletDom.firstChildElement(QString("Reserved"));
        module_.m_bReserved = ele.attribute(QString("val")).toInt() ? true : false;
        module_.m_reservedBy = ele.attribute(QString("name"));

        ele = moduletDom.firstChildElement(QString("ports"));
        {
            QDomElement portEle = ele.firstChildElement(QString("port"));
            module_.m_ports.clear();
            while(!portEle.isNull()) {
                CPort* port_ = new CPort();
                if (CPort::fromXml(portEle, *port_)) {
                    module_.appendPort(port_);
                }
                else {
                    return false;
                }
                portEle = portEle.nextSiblingElement();
            }
        }
    }
    catch(...)
    {

        return false;
    }

    return true;
}
