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

        RL_C_PORT_COUNTS,         //  机箱所有板卡的端口信息
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

        RL_PS_COMMENT,
        RL_PS_MODIFIERCOUNT,
        RL_PS_PACKETLIMIT,
        RL_PS_RATEFRACTION,
        RL_PS_RATEPPS,
        RL_PS_ENABLE,
        RL_PT_TOTAL,
    };

    bool parse(const QString &text);

    Result result() {return m_result;}
    const QString &text() const {return m_text;}
    int moduleIndex() const {return m_mIndex;}
    int portIndex() const {return m_pIndex;}
    int streamIndex() const {return m_sIndex;}

    void reset();


private:    
    bool dealAbnormalReply(const QString & receiveData);                // 处理异常的回复信息
    bool dealSingleReply(const QStringList & singleList);               // 处理单个回复信息
    bool dealChassisReply(const QStringList & chassisInfoList);         // 处理机箱的回复信息
    bool dealModuleAndPortReply(const QStringList & infoList);          // 处理板卡和端口的回复信息
    bool dealModuleReply(const QStringList & moduleList);               // 处理板卡的回复信息
    bool dealPortReply(const QStringList & portInfoList);               // 处理端口的回复信息
    bool dealStreamReply(const QStringList & streamInfoList);           // 处理流的回复信息
    bool dealOtherReply(const QStringList & replyList);                 // 处理其他回复信息
    bool dealPortStatisticsReply(const QStringList & statisticsList);   // 处理端口统计的回复信息

private:
    Result m_result;

    int m_mIndex;
    int m_pIndex;
    int m_sIndex;
    QString m_text;

    friend class CDeviceImpl;
    friend class IDevice;
};

