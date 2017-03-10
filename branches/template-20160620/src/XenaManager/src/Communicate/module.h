#pragma once

#include <QList>
#include <QString>

class CChassis;
class CPort;
class CModule
{
public:
    CModule(CChassis* chassis = NULL);
    ~CModule();

    void dumpMsg();

    CChassis* chassis() const {return m_chassis;}
    int itemOfChassis() const;
    void setChassis(CChassis* chassis_) {m_chassis = chassis_;}

    const QString & name() const {return m_name;}
    void setName(const QString &name_) {m_name = name_;}

    quint16 portCount() const {return m_ports.size();}

    /*
     *  @Func:  port
     *  
     *  @Param[in]:
     *      item:
     *  
     *  @Return:
     *      NULL:       the size of m_ports is less than item
     *      CModule*:   the pointer of CPort
     */
    CPort* port(quint16 item) const;

    /*
     *  @Func:  findPort
     *  
     *  @Param[in]:
     *      port: 
     *  
     *  @Return:
     *      -1:   not found 
     *     >=0:  found
     */
    int findPort(const CPort *port_) const;

    bool appendPort(CPort *port_);
    
private:
    CChassis* m_chassis;
    
    QString   m_name;

    QList<CPort*> m_ports;
};