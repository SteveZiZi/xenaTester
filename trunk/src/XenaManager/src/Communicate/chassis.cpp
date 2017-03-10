#include "chassis.h"
#include "module.h"

#include <QDebug>

CChassis::CChassis()
: m_chassisName("unknown")
, m_chassisSerailNo("12345678")
{

}

CChassis::~CChassis()
{
    Q_FOREACH(CModule* module_, m_modules)
        if (module_) delete module_;
}


CModule* CChassis::module(quint16 item) const
{
    if (item < m_modules.size()) {
        return m_modules.at(item);
    }
    return NULL;
}


bool CChassis::appendModule(CModule* module)
{
    if (!module) {
         return false;
    }

    m_modules.push_back(module);
    module->setChassis(this);
    return true;
}

int CChassis::findModule(const CModule* module) const
{
    if (!module) {
        return -1;
    }

    int item = 0;
    Q_FOREACH(CModule* m, m_modules) {
        if (m == module) {
            return item;
        }
        item++;
    }

    return -1;
}

void CChassis::dumpMsg()
{
    qDebug( "++++++++++++  chassis  ++++++++++");

    qDebug() << "chassisName:" << m_chassisName;
    qDebug() << "chassisSerialNo:" << m_chassisSerailNo;

    Q_FOREACH(CModule* m, m_modules)
        m->dumpMsg();

    qDebug( "------------  chassis  ----------");
}


CPort* CChassis::port(int moduleIndex, int portIndex)
{
    if (moduleIndex < m_modules.size()) {
        return port(m_modules.at(moduleIndex), portIndex);
    }

    return NULL;
}

CPort* CChassis::port(CModule* module_, int portIndex)
{
    if (module_ && portIndex < module_->portCount()) {
        return module_->port(portIndex);
    }

    return NULL;
}