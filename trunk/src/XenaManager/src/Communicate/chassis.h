#pragma once

#include <QString>
#include <QList>

class CModule;
class CPort;

class CChassis
{
public:
    CChassis();
    ~CChassis();

    void dumpMsg();

    const QString & chassisName() const {return m_chassisName;}
    void setChassisName(const QString &name) {m_chassisName = name;}
    const QString & chassisSerialNo() const {return m_chassisSerailNo;}
    void setChassisSerialNo(const QString &serialNo) {m_chassisSerailNo = serialNo;}

    bool reserved() const {return m_bReserved;}
    void setReserved(bool reserved_) {m_bReserved = reserved_;}

    const QString &reservedBy() const {return m_reservedBy;}
    void setReservedBy(const QString owner) {m_reservedBy = owner;}


    quint16 moduleCount() const {return m_modules.size();}

    /*
     *  @Func:  module
     *  
     *  @Param[in]:
     *      item:
     *  
     *  @Return:
     *      NULL:       the size of m_modules is less than item
     *      CModule*:   the pointer of CModule
     */
    CModule* module(quint16 item) const;
    /*
     *  @Func:  findModule
     *  
     *  @Param[in]:
     *      module: 
     *  
     *  @Return:
     *      -1:   not found 
     *     >=0:  found
     */
    int findModule(const CModule* module) const;

    bool appendModule(CModule* module);

    /*
     *  @Func:  port
     *  
     *  @Param[in]:
     *      moduleIndex:    �忨��
     *      portIndex:      �˿ں�
     *  
     *  @Return:
     *      CPortָ�룺     �ɹ����ض�Ӧ�忨��ָ���˿ںŵ�CPort����
     *      NULL:           �忨�Ż��߶˿ںŴ���
     */
    CPort* port(int moduleIndex, int portIndex);
    CPort* port(CModule* module_, int portIndex);
    
private:
    QString         m_chassisName;
    QString         m_chassisSerailNo;
    QList<CModule*> m_modules;

    bool      m_bReserved;
    QString   m_reservedBy;

};