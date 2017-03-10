#pragma once

#include <QList>
#include <QString>
#include <QDomDocument>


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
     *  ����һ��δ��ʹ�õ������
     */
    int unUsedIndexOfStream();

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
     *  @Func:  �Ƴ�Stream
     *  
     *  @Param[in]:
     *      stream_:  Ҫ�Ƴ���stream��ָ��
     *  
     *  @Return:
     *      CStream*:   �����Ƴ���stream��ָ�� 
     *      NULL    :   fail �޴�stream
     *  @Note:
     *      ��Ҫ���ⲿ�ͷŸ�stream��Դ
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
    void setPortName(const QString &name);

    bool isBadPort() {return m_isBadPort;}
    void setBadPort(bool bad) {m_isBadPort = bad;}

    // ����Ϊ�˿��������
    QString txMode() const {return m_sTxMode;}
    void setTxMode(const QString &txMode_) {m_sTxMode = txMode_;}

    float rateFraction() const {return m_fRateFraction;}
    void setRateFraction(const float rateFraction_) {m_fRateFraction = rateFraction_;}

    int packetRate() const {return m_iPacketRate;}
    void setPacketRate(const int packetRate_) {m_iPacketRate = packetRate_;}

    double bitRate() const {return m_dBitRate;}
    void setBitRate(const double bitRate_) {m_dBitRate = bitRate_;}


    static bool toXml(const CPort *port, QDomDocument &doc, QDomElement &portDom);
    static  bool fromXml(QDomElement &portDom, CPort &port);


private:
    CModule* m_module;
    QList<CStream*> m_streams;

    bool    m_isBadPort;

    int     m_nMaxHeaderLength;
    int     m_nMaxStreamNum;
    int     m_nTxTimeLimit;
    int     m_nRealSpeed;
    bool    m_bLinked;
    bool    m_bActived;
    bool    m_bReserved;
    QString m_reservedBy;

    // ����Ϊ�˿��������
    QString m_sTxMode;
    float   m_fRateFraction;
    int     m_iPacketRate;
    double  m_dBitRate;

    QString m_portName;
};