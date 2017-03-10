#pragma once

#include <QString>
#include <QVector>

class CXenaParser
{
public:
    CXenaParser() {}

    enum Result {
        RL_ERROR_SYNTAX_ERROR = -127,
        RL_ERROR_NOT_RESERVED,
        RL_ERROR_NOT_READABLE,
        RL_ERROR_NOT_WRITEABLE,
        RL_ERROR_NOT_VALID,
        RL_ERROR_BAD_MODULE,
        RL_ERROR_BAD_INDEX,
        RL_ERROR_BAD_SIZE,
        RL_ERROR_BAD_VALUE,
        RL_ERROR_FAILED,
        RL_ERROR_BAD_PORT,

        RL_UNKNOWN = -1,
        RL_OK = 0,
        RL_NO_REPLY,

        RL_C_PORT_COUNTS,         //  �������а忨�Ķ˿���Ϣ
        RL_C_KEEP_ALIVE,
        RL_C_NAME,
        RL_C_SERIALNO,
        RL_C_RESERVATION,
        RL_C_RESERVEDBY,

        RL_M_MODEL,
        RL_M_RESERVATION,
        RL_M_RESERVEDBY,

        RL_P_INTERFACE,
        RL_P_COMMENT,
        RL_P_RESERVATION,
        RL_P_RECEIVESYNC,
        RL_P_TRAFFIC,
        RL_P_TPLDMODE,
        RL_P_CHECKSUM,
        RL_P_MAXHEADERLENGTH,
        RL_P_RESERVEDBY,
        RL_P_TXTIMELIMIT,
        RL_P_SPEED,
        RL_P_TXMODE,
        RL_P_RATEFRACTION,
        RL_P_RATEPPS,
        RL_P_RATEL2BPS,

        RL_PS_COMMENT,
        RL_PS_MODIFIERCOUNT,
        RL_PS_PACKETLIMIT,
        RL_PS_RATEFRACTION,
        RL_PS_RATEPPS,
        RL_PS_ENABLE,
        RL_PS_TPLDID,
        RL_PS_PACKETHEADER,
        RL_PS_HEADERPROTOCOL,
        RL_PS_PACKETLENGTH,
        RL_PS_MODIFIER,
        RL_PS_MODIFIERRANGE,
        RL_PS_STREAM_NUM,

        RL_PT_TOTAL,
    };

    bool parse(const QString &text);

    Result result() {return m_result;}
    const QString &text() const {return m_text;}
    int moduleIndex() const {return m_mIndex;}
    int portIndex() const {return m_pIndex;}
    int streamIndex() const {return m_sIndex;}
    int modifierIndex() const {return m_miIndex;}

    void reset();


private:    
    bool dealAbnormalReply(const QString & receiveData);                // �����쳣�Ļظ���Ϣ
    bool dealSingleReply(const QStringList & singleList);               // �������ظ���Ϣ
    bool dealChassisReply(const QStringList & chassisInfoList);         // �������Ļظ���Ϣ
    bool dealModuleAndPortReply(const QStringList & infoList);          // ����忨�Ͷ˿ڵĻظ���Ϣ
    bool dealModuleReply(const QStringList & moduleList);               // ����忨�Ļظ���Ϣ
    bool dealPortReply(const QStringList & portInfoList);               // ����˿ڵĻظ���Ϣ
    bool dealStreamReply(const QStringList & streamInfoList);           // �������Ļظ���Ϣ
    bool dealOtherReply(const QStringList & replyList);                 // ���������ظ���Ϣ
    bool dealPortStatisticsReply(const QStringList & statisticsList);   // ����˿�ͳ�ƵĻظ���Ϣ
    bool dealStreamAbnormalReply(const QStringList & streamCountList);  // ������������ʽ�ظ�������

    void removeInvisibleSymbols(QString &text);

private:
    Result m_result;

    int m_mIndex;
    int m_pIndex;
    int m_sIndex;
    int m_miIndex;                                                      // ����������
    QString m_text;

    friend class CDeviceImpl;
    friend class IDevice;
};

