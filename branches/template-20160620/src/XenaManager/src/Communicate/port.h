#pragma once

#include <QList>
#include <QString>


class CModule;
class CStream;
class CPort
{
public:
    CPort(CModule* _module = NULL);
    ~CPort();

    void dumpMsg();

    CModule* module() const {return m_module;}
    void setModule(CModule* module_) {m_module = module_;}
    int itemOfModule() const;

    quint16 streamCount() const {return m_streams.size();}

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
    CStream* stream(quint16 item) const;

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
    int findStream(const CStream *stream_) const;

    bool appendStream(CStream *stream_);

    /*
     *  @Func:  移除Stream
     *  
     *  @Param[in]:
     *      stream_:  要移除的stream的指针
     *  
     *  @Return:
     *      CStream*:   返回移除的stream的指针 
     *      NULL    :   fail 无此stream
     *  @Note:
     *      需要在外部释放该stream资源
     */
    CStream* removeStream(CStream *stream_);
    CStream* removeStream(int streamIndex);

    int maxHeaderLength() const {return m_nMaxHeaderLength;}
    int maxStreamNum() {return m_nMaxStreamNum;}
    void setMaxHeaderLength(int maxLen);

    int txTimeLimit() const {return m_nTxTimeLimit;}
    void setTxTimeLimit(int limitTm) {m_nTxTimeLimit = limitTm;}

    int realSpeed() const {return m_nRealSpeed;}
    void setRealSpeed(int speed) {m_nRealSpeed = speed;}

    bool linked() const {return m_bLinked;}
    void setLinked(bool linked_) {m_bLinked = linked_;}
    bool actived() const {return m_bActived;}
    void setActived(bool actived_) {m_bActived = actived_;}

    bool reserved() const {return m_bReserved;}
    void setReserved(bool reserved_) {m_bReserved = reserved_;}

    const QString &reservedBy() const {return m_reservedBy;}
    void setReservedBy(const QString owner) {m_reservedBy = owner;}

    const QString & portName() const {return m_portName;}
    void setPortName(const QString &name) {m_portName = name;}


private:
    CModule* m_module;
    QList<CStream*> m_streams;

    int     m_nMaxHeaderLength;
    int     m_nMaxStreamNum;
    int     m_nTxTimeLimit;
    int     m_nRealSpeed;
    bool    m_bLinked;
    bool    m_bActived;
    bool    m_bReserved;
    QString m_reservedBy;

    QString m_portName;
};