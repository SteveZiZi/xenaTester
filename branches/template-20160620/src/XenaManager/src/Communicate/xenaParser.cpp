#include "xenaParser.h"
#include <QStringList>
#include <QDebug>

enum {
    SIGNALREPLY = 1,
    CHASSISREPLY,
    MODULANDPORTEREPLY,
    STREAMREPLY
};


bool CXenaParser::parse(const QString &text)
{
    reset();

    if(text.contains("#Syntax error",Qt::CaseInsensitive))
    {
        m_result = RL_ERROR_SYNTAX_ERROR;
        return false;
    }

    QStringList splitList;
    splitList = text.split("  ");
    switch(splitList.size())
    {
    case SIGNALREPLY:
        return dealSingleReply(splitList);
    case CHASSISREPLY:
        return dealChassisReply(splitList);
    case MODULANDPORTEREPLY:
        return dealModuleAndPortReply(splitList);
    case STREAMREPLY:
        return dealStreamReply(splitList);
    default:
        return dealOtherReply(splitList);
    }

    return false;
}

void CXenaParser::reset()
{
    m_mIndex = -1;
    m_pIndex = -1;
    m_sIndex = -1;
    m_text.clear();
    m_result = RL_NO_REPLY;
}


bool CXenaParser::dealAbnormalReply(const QString & receiveData)
{
    qDebug() << "******* abnormal reply";

    return false;
}

bool CXenaParser::dealSingleReply(const QStringList & singleList)
{
    const QString &tempStr = singleList[0];
    if (tempStr.contains("OK",Qt::CaseInsensitive)) {
        m_result = RL_OK;
    }
    else if (tempStr.contains("NOTRESERVED",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_NOT_RESERVED;
    }
    else if (tempStr.contains("NOTREADABLE",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_NOT_READABLE;
    }
    else if (tempStr.contains("NOTWRITABLE",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_NOT_WRITEABLE;
    }
    else if (tempStr.contains("NOTVALID",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_NOT_VALID;
    }
    else if (tempStr.contains("BADMODULE",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_BAD_MODULE;
    }
    else if (tempStr.contains("BADINDEX",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_BAD_INDEX;
    }
    else if (tempStr.contains("BADSIZE",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_BAD_SIZE;
    }
    else if (tempStr.contains("BADVALUE",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_BAD_VALUE;
    }
    else if (tempStr.contains("FAILED",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_FAILED;
    }
    else if (tempStr.contains("BADPORT",Qt::CaseInsensitive)) {
        m_result = RL_ERROR_BAD_PORT;
    }
    else {
        return false;
    }

    return true;
}

bool CXenaParser::dealChassisReply(const QStringList & chassisInfoList)
{
    const QString & commandStr = chassisInfoList[0];
    const QString & signalStr = chassisInfoList[1];

    if (!commandStr.contains("C_",Qt::CaseInsensitive))
    {
        return false;
    }

    m_text = signalStr;
    if (commandStr.contains("C_KEEPALIVE",Qt::CaseInsensitive)) {
        m_result = RL_C_KEEP_ALIVE;
    }
    else if (commandStr.contains("C_NAME",Qt::CaseInsensitive)) {
        m_result = RL_C_NAME;
    }
    else if (commandStr.contains("C_SERIALNO",Qt::CaseInsensitive)) {
        m_result = RL_C_SERIALNO;
    }
    else if (commandStr.contains("C_RESERVATION",Qt::CaseInsensitive)) {
        m_result = RL_C_RESERVATION;
    }
    else if(commandStr.contains("C_RESERVEDBY",Qt::CaseInsensitive)) {
        m_result = RL_C_RESERVEDBY;
    }
    else if (commandStr.contains("C_PORTCOUNTS",Qt::CaseInsensitive)) {
        m_result = RL_C_PORT_COUNTS;
    }
    else {
        return false;
    }

    return true;
}

bool CXenaParser::dealModuleAndPortReply(const QStringList & infoList)
{
    QString moduleAndPortInfo = infoList[1];
    if (moduleAndPortInfo.contains("M_",Qt::CaseInsensitive))
    {
        return dealModuleReply(infoList);
    }
    else if (moduleAndPortInfo.contains("P_",Qt::CaseInsensitive))
    {
        return dealPortReply(infoList);
    }
    else if (moduleAndPortInfo.contains("PT_",Qt::CaseInsensitive))
    {
        return dealPortStatisticsReply(infoList);
    }

    return false;
}

bool CXenaParser::dealModuleReply(const QStringList & moduleList)
{
    const QString &commandStr = moduleList[1];
    int moduleIndex = moduleList[0].toInt();
    if (moduleIndex < 0) {
        return false;
    }
    const QString &signalStr = moduleList[2];

    m_mIndex = moduleIndex;
    m_text = signalStr;
    if (commandStr.contains("M_MODEL",Qt::CaseInsensitive)) {
        m_result = RL_M_MODEL;
    }
    else if (commandStr.contains("M_RESERVATION",Qt::CaseInsensitive)) {
        m_result = RL_M_RESERVATION;
    }
    else if (commandStr.contains("M_RESERVEDBY",Qt::CaseInsensitive)) {
        m_result = RL_M_RESERVEDBY;
    }
    else {
        return false;
    }

    return true;
}

bool CXenaParser::dealPortReply(const QStringList & portInfoList)
{
    const QString &commandStr = portInfoList[1];
    const QString &moduleIndexAndPortIndex = portInfoList[0];

    if (moduleIndexAndPortIndex.isEmpty() || 
        !moduleIndexAndPortIndex.contains("/"))
    {
        return false;
    }

    QStringList moduleAndPortList = portInfoList[0].split("/");
    int moduleIndex = moduleAndPortList[0].toInt();
    int portIndex = moduleAndPortList[1].toInt();
    const QString &signalStr = portInfoList[2];

    m_mIndex = moduleIndex;
    m_pIndex = portIndex;
    m_text = signalStr;
    if (commandStr.contains("P_INTERFACE",Qt::CaseInsensitive)) {
        m_result = RL_P_INTERFACE;
    }
    else if (commandStr.contains("P_COMMENT",Qt::CaseInsensitive)) {
        m_result = RL_P_COMMENT;
    }
    else if (commandStr.contains("P_RESERVATION",Qt::CaseInsensitive)) {
        m_result = RL_P_RESERVATION;
    }
    else if (commandStr.contains("P_RECEIVESYNC",Qt::CaseInsensitive)) {
        m_result = RL_P_RECEIVESYNC;
    }
    else if (commandStr.contains("P_TRAFFIC",Qt::CaseInsensitive)) {
        m_result = RL_P_TRAFFIC;
    }
    else if (commandStr.contains("P_TPLDMODE",Qt::CaseInsensitive)) {
        m_result = RL_P_TPLDMODE;
    }
    else if (commandStr.contains("P_CHECKSUM",Qt::CaseInsensitive)) {
        m_result = RL_P_CHECKSUM;
    }
    else if (commandStr.contains("P_MAXHEADERLENGTH",Qt::CaseInsensitive)) {
        m_result = RL_P_MAXHEADERLENGTH;
    }
    else if (commandStr.contains("P_RESERVEDBY",Qt::CaseInsensitive)) {
        m_result = RL_P_RESERVEDBY;
    }
    else if (commandStr.contains("P_TXTIMELIMIT",Qt::CaseInsensitive)) {
        m_result = RL_P_TXTIMELIMIT;
    }
    else if (commandStr.contains("P_SPEED",Qt::CaseInsensitive)) {
        m_result = RL_P_SPEED;
    }
    else {
        return false;
    }

    return true;
}

static inline int getNumberFormString(const QString &str)
{
    QString tempStr;
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i].isDigit())
        {
            tempStr.append(str[i]);
        }
    }
    return tempStr.toInt();
}

bool CXenaParser::dealStreamReply(const QStringList & streamInfoList)
{
    Q_ASSERT(streamInfoList.size() == 4);

    const QString &commandStr = streamInfoList[1];
    const QString &moduleIndexAndPortIndex = streamInfoList[0];
    if ( moduleIndexAndPortIndex.isEmpty() ||
        !moduleIndexAndPortIndex.contains("/") ||
        !commandStr.contains("PS_",Qt::CaseInsensitive))
    {
        return false;
    }

    QStringList moduleAndPortList = streamInfoList[0].split("/");
    int moduleIndex = moduleAndPortList[0].toInt();
    int portIndex = moduleAndPortList[1].toInt();
    int streamIndex = getNumberFormString(streamInfoList[2]);
    const QString &signalStr = streamInfoList[3];

    m_mIndex = moduleIndex;
    m_pIndex = portIndex;
    m_sIndex = streamIndex;
    m_text = signalStr;
    if (commandStr.contains("PS_COMMENT",Qt::CaseInsensitive)) {
        m_result = RL_PS_COMMENT;
    }
    else if (commandStr.contains("PS_MODIFIERCOUNT",Qt::CaseInsensitive)) {
        m_result = RL_PS_MODIFIERCOUNT;
    }
    else if (commandStr.contains("PS_PACKETLIMIT",Qt::CaseInsensitive)) {
        m_result = RL_PS_PACKETLIMIT;
    }
    else if (commandStr.contains("PS_RATEFRACTION",Qt::CaseInsensitive)) {
        m_result = RL_PS_RATEFRACTION;
    }
    else if (commandStr.contains("PS_RATEPPS",Qt::CaseInsensitive)) {
        m_result = RL_PS_RATEPPS;
    }
    else if (commandStr.contains("PS_ENABLE",Qt::CaseInsensitive)) {
        m_result = RL_PS_ENABLE;
    }
    else {
        return false;
    }

    return true;
}

bool CXenaParser::dealOtherReply(const QStringList & replyList)
{
    return false;
}

bool CXenaParser::dealPortStatisticsReply(const QStringList & statisticsList)
{
    QString commandStr = statisticsList[1];
    QString moduleIndexAndPortIndex = statisticsList[0];
    if (moduleIndexAndPortIndex.isEmpty() || 
        !moduleIndexAndPortIndex.contains("/"))
    {
        return false;
    }

    QStringList moduleAndPortList = statisticsList[0].split("/");
    int moduleIndex = moduleAndPortList[0].toInt();
    int portIndex = moduleAndPortList[1].toInt();
    const QString signalStr = statisticsList[2];

    m_mIndex = moduleIndex;
    m_pIndex = portIndex;
    m_text = signalStr;

    if (commandStr.contains("PT_TOTAL",Qt::CaseInsensitive))
    {
        m_result = RL_PT_TOTAL;
    }
    else
    {
        return false;
    }

    return true;
}