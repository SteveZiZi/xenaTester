#pragma once

#include <QList>
#include <QString>
#include <QDomDocument>

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
    void setName(const QString &name_);


    bool reserved() const {return m_bReserved;}
    void setReserved(bool reserved_) {m_bReserved = reserved_;}

    const QString &reservedBy() const {return m_reservedBy;}
    void setReservedBy(const QString owner) {m_reservedBy = owner;}

    bool isBadModule() {return m_bBadModule;}
    void setBadModule(bool bad) {m_bBadModule = bad;}


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

    static bool toXml(const CModule *module_, QDomDocument &doc, QDomElement &moduletDom);
    static bool fromXml(QDomElement &moduletDom, CModule &module_);
    
private:
    CChassis* m_chassis;

    bool      m_bBadModule;
    
    QString   m_name;
    bool      m_bReserved;
    QString   m_reservedBy;

    QList<CPort*> m_ports;
};