#include "xenaTester.h"
#include "device.h"
#include <QDebug>

CXenaTester::CXenaTester()
{
    m_commDev = GetCommDevice();
	m_bLogin = false;
}

CXenaTester::~CXenaTester()
{

}

bool  CXenaTester::login(const QString &ip, int port, const QString &userName, const QString &pwd)
{
    m_bLogin = m_commDev->login(ip, port, userName, pwd);

    return m_bLogin;
}

void  CXenaTester::logout()
{
    if (m_bLogin) {
        m_commDev->logout();
        m_bLogin = false;
    }
}

bool  CXenaTester::queryChassisInfo()
{
#if 0
    return m_commDev->acquireChassisInfo(&m_chassis);
#else
    bool rc = true;
    QString str;
    m_commDev->chassisName(str);
    m_chassis.setChassisName(str);
    m_commDev->chassisSerialNo(str);
    m_chassis.setChassisSerialNo(str);

    int mCnt = m_commDev->moduleCount();
    if (mCnt == -1) {
        return false;
    }
    for(int m = 0; m < mCnt; m++) {
        CModule* module = new CModule();
        if (m_commDev->moduleName(m, str))
        {
            module->setName(str);
            int pCnt =m_commDev->portCount(m);
            for(int p = 0; p < pCnt; p++) {
                CPort* port = new CPort();
                if (m_commDev->portName(m, p, str))
                {
                    port->setPortName(str);
                    port->setReserved(m_commDev->portReservedOwner(m, p, str));
                    port->setReservedBy(str);
                    bool link, active;
                    m_commDev->portCommunicationStatus(m, p, link, active);
                    port->setLinked(link);
                    port->setActived(active);
                    int tmp;
                    m_commDev->portMaxHeaderLength(m, p, tmp);
                    port->setMaxHeaderLength(tmp);
                    m_commDev->portTimeLimit(m, p, tmp);
                    port->setTxTimeLimit(tmp);
                    m_commDev->portRealSpeed(m, p, tmp);
                    port->setRealSpeed(tmp);
                }
                module->appendPort(port);
            }
        }
        m_chassis.appendModule(module);
    }
    return rc;
#endif
}


/************************************************************************/
/* 机箱操作                                                             */
/************************************************************************/
const QString & CXenaTester::chassisSerialNo()
{
    Q_ASSERT(m_bLogin);
    return m_chassis.chassisSerialNo();
}

bool CXenaTester::reservedChassis()
{
    return m_commDev->reserveChassis();
}

/************************************************************************/
/* 板卡操作                                                             */
/************************************************************************/
bool CXenaTester::reservedModule(int moduleIndex)
{
    return m_commDev->reserveModule(moduleIndex);
}

/************************************************************************/
/* 端口操作                                                             */
/************************************************************************/
bool CXenaTester::reservedPort(int moduleIndex, int portIndex)
{
    return m_commDev->reservePort(moduleIndex, portIndex);
}


bool CXenaTester::addStream(int moduleIndex, int portIndex, const ImportData *data)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    if (!port || port->streamCount() >= port->maxStreamNum()) {
        return false;
    }
    
    CStream* stream = new CStream(port);
    stream->setData(data);
    if (data->frameStyle.contains("SMV92", Qt::CaseInsensitive)) {
        stream->setTxFramesPerSecond(4000);
    }
    else {
        stream->setTxFramesPerSecond(1);
    }
    QString name = QString("Stream%1 %2[%3]").arg(port->streamCount())
                                             .arg(data->frameStyle)
                                             .arg(data->appId, 4, 16, QChar('0'));
    if (data->iedName != "UNKNOWN") {
        name += " ";
        name += data->iedName;
    }
    stream->setStreamName(name);

    if (m_commDev->addStream(moduleIndex, portIndex, stream)) {
        port->appendStream(stream);
        return true;
    }

    delete stream;
    return false;
}

bool CXenaTester::addStream(int moduleIndex, int portIndex, CStream *stream)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    if (!port || port->streamCount() >= port->maxStreamNum()) {
        return false;
    }

    if (m_commDev->addStream(moduleIndex, portIndex, stream)) {
        port->appendStream(stream);
        return true;
    }

    return false;
}

//  端口是否正在发送数据
bool CXenaTester::portIsTrafficing(int moduleIndex, int portIndex)
{
    int bps, pps;
    if (m_commDev->portTxStatistics(moduleIndex, portIndex, bps, pps)) {
        return (0 != bps || 0 != pps);
    }

    return false;
}
bool CXenaTester::portIsTrafficing(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portIsTrafficing(moduleIndex, portIndex);
}


bool CXenaTester::portStartSpeedTraffic(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    m_commDev->setPortTimeLimit(moduleIndex, portIndex, port->txTimeLimit());
    for(int i = 0; i < port->streamCount(); i++) {
        CStream*stream = port->stream(i);
        if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
            m_commDev->setStreamRate(moduleIndex, portIndex, i, stream->txPercent());
        }
        else {
            m_commDev->setStreamRate(moduleIndex, portIndex, i, stream->txFramesPerSecond());
        }
        m_commDev->setStreamPacketLimit(moduleIndex, portIndex, i, stream->packetLimit());
        //  暂时不重新设置跳变域
    }

    bool traffic = true;
    m_commDev->setPortTraffic(moduleIndex, portIndex, traffic);
    port->setActived(traffic);

    return true;
}
bool CXenaTester::portStartSpeedTraffic(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portStartSpeedTraffic(moduleIndex, portIndex);
}

bool CXenaTester::portNextSpeedTraffic(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    m_commDev->setPortTimeLimit(moduleIndex, portIndex, port->txTimeLimit());
    int txStream = 0;
    for(int i = 0; i < port->streamCount(); i++) {
        CStream*stream = port->stream(i);
        bool ok = false;
        if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
            float percent = stream->nextPercent(&ok);
            stream->setTxPercent(percent);
            qDebug() << moduleIndex << "/" << portIndex << "percent:" << percent;
        }
        else {
            int frames = stream->nextFramesPerSecond(&ok);
            stream->setTxFramesPerSecond(frames);
            qDebug() << moduleIndex << "/" << portIndex << "/" << i << "FramesPerSecond:" << frames;
        }
        if (ok) txStream++;
        //  暂时不重新设置跳变域
    }

    return (txStream != 0) ? true : false;
}
bool CXenaTester::portNextSpeedTraffic(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portNextSpeedTraffic(moduleIndex, portIndex);
}

bool CXenaTester::portStopTranffic(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    bool traffic = false;
    m_commDev->setPortTraffic(moduleIndex, portIndex, traffic);
    port->setActived(traffic);

    return true;
}
bool CXenaTester::portStopTranffic(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portStopTranffic(moduleIndex, portIndex);
}


//  端口完成策略发送
bool CXenaTester::portFinishedStrategy(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    for(int i = 0; i < port->streamCount(); i++) {
        CStream*stream = port->stream(i);
        stream->resetStrategyStatus();
    }
    return true;
}

bool CXenaTester::portFinishedStrategy(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portFinishedStrategy(moduleIndex, portIndex);
}