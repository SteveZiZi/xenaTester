#include <QFile>
#include "deviceImpl.h"

#include "xenaParser.h"
#include "chassis.h"
#include "module.h"
#include "port.h"
#include "stream.h"

#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"

#include <QMessageBox>
#include <QApplication>

#include <QtDebug>

#define DEBUG_CONTEXT 0

#if DEBUG_CONTEXT
#define DEBUG_CONTEXT_FILE ".\\debugContext\\debugContext"
bool writeDebugConetext(const QString text)
{
    QFile file(DEBUG_CONTEXT_FILE);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "open debugContext fail";
        return false;
    }
    QTextStream out(&file);
    out << text;
    file.close();
}
#endif

static CDeviceImpl m_deviceImpl;
/*
 *  获取通信设备
 *  @Note:
 *      返回的是一个静态全局指针，调用者不需要释放设备
 */
IDevice* GetCommDevice()
{
    return &m_deviceImpl;
}


CDeviceImpl::CDeviceImpl(QObject* parent/* = 0*/) 
: QObject(parent)
, m_bLogin(false)
, m_parser(NULL)
, m_bQueryPortCount(false)
, m_bSocketExceptions(false)
, m_bExitThread(false)
, m_bSuspendThread(false)
, m_pollTimer(NULL)
{
    m_pTcpSocket = new QTcpSocket(this);
    connect(m_pTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    m_parser = new CXenaParser();
}

CDeviceImpl::~CDeviceImpl()
{
    if (m_pollTimer && m_pollTimer->isActive()) {
        m_pollTimer->stop();
    }
    if (m_parser) {
        delete m_parser;
        m_parser = NULL;
    }
}


void CDeviceImpl::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::warning(0,tr("Warning"),tr("Disconnected,Please check the network!"),QString(tr("OK")));
        m_bSocketExceptions = true;
        break;
    case QAbstractSocket::RemoteHostClosedError:
        QMessageBox::warning(0,tr("Warning"),tr("Disconnected,Please check the network!"),QString(tr("OK")));
        m_bSocketExceptions = true;
        qApp->exit(0);
        break;
    case QAbstractSocket::SocketTimeoutError:
        //QMessageBox::warning(0,tr("Warning"),tr("Timeout,Please check the network!"),QString(tr("OK")));
       // m_bSocketExceptions = true;
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::warning(0,tr("Warning"),tr("Connected fail,Please check the Ip and port!"),QString(tr("OK")));
        m_bSocketExceptions = true;
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::warning(0,tr("Warning"),tr("Connected fail,Please check the Ip and port!"),QString(tr("OK")));
        m_bSocketExceptions = true;
        break;
    default:
        break;
    }
}


bool CDeviceImpl::login(const QString &ip, int port, const QString &userName, const QString &pwd)
{
    QMutexLocker _lock(&m_mutex);

    Q_ASSERT(m_pTcpSocket);
    m_bLogin = false;
    int nTry = 0;
tryAgain:
    m_pTcpSocket->abort();
    m_pTcpSocket->connectToHost(ip,port);
    if(!m_pTcpSocket->waitForConnected(XenaManageSettings::waitConnectTime())) {
        nTry++;
        if(nTry <= 5) {
            goto tryAgain;
        }
        qDebug() << "disConnected!";
        return false;
    }
    m_bSocketExceptions = false;

    m_cmdMgr.setUserName(userName);
    m_cmdMgr.setPassword(pwd);

    this->receiveData();
    if (this->sendData(C_LOGON) && this->waitForRead(-1)) {
        QString text;
        this->receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK) {
            if (this->sendData(C_OWNER) && this->waitForRead(-1)) {
                this->receiveData();
                m_bLogin = true;
                m_bQueryPortCount = false;
            }
        }
    }
    
    return m_bLogin;
}

bool CDeviceImpl::logout()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    this->sendData(C_LOGOFF);
    m_pTcpSocket->disconnectFromHost();
    m_bLogin = false;
    return true;
}

bool CDeviceImpl::keepAlive()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    return this->sendData(C_KEEPALIVE);
}

bool CDeviceImpl::reserveChassis()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    if (sendData(C_RESERVATION_RESERVE) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

bool CDeviceImpl::releaseChassis()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    if (sendData(C_RESERVATION_RELEASE) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

bool CDeviceImpl::relinquishChassis()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    if (sendData(C_RESERVATION_RELINQUISH) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

bool CDeviceImpl::reserveModule(int moduleIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;

    if (sendData(M_RESERVATION_RESERVE,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

bool CDeviceImpl::releaseModule(int moduleIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;

    if (sendData(M_RESERVATION_RELEASE,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

bool CDeviceImpl::relinquishModule(int moduleIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;

    if (sendData(M_RESERVATION_RELINQUISH,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}


bool CDeviceImpl::reservePort(int moduleIndex, int portIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_RESERVATION_RESERVE,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

bool CDeviceImpl::releasePort(int moduleIndex,int portIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_RESERVATION_RELEASE,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

bool CDeviceImpl::relinquishPort(int moduleIndex,int portIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_RESERVATION_RELINQUISH,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK)
        {
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}


void CDeviceImpl::setPortTraffic(int moduleIndex,int portIndex,bool & tx)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (tx)
    {
        sendData(P_TRAFFIC_ON,order);
    }
    else
    {
        sendData(P_TRAFFIC_OFF,order);
    }
}

bool CDeviceImpl::portMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_MAXHEADERLENGTH_QUERY,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_MAXHEADERLENGTH)
        {
            maxLen = m_parser->text().toInt();
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

void CDeviceImpl::setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.data.setNum(maxLen);
    sendData(P_MAXHEADERLENGTH_SET, order);
    waitForRead();
}

bool CDeviceImpl::portTimeLimit(int moduleIndex, int portIndex, int &limitMs)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_TXTIMELIMIT_QUERY,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_TXTIMELIMIT)
        {
            limitMs = m_parser->text().left(m_parser->text().size() - 3).toInt();
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

void CDeviceImpl::setPortTimeLimit(int moduleIndex, int portIndex, int limitMS)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.data = QString::number(limitMS);
    order.data += "000";
    sendData(P_TXTIMELIMIT_SET, order);
}

void CDeviceImpl::setPortRealSpeed(int moduleIndex, int portIndex, int speed_mbps)
{
    Q_ASSERT(false);
}

bool CDeviceImpl::portTxMode(int moduleIndex, int portIndex, QString &mode)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_TXMODE_QUERY,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_TXMODE)
        {
            mode = m_parser->text();
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

void CDeviceImpl::setPortTxModeNormal(int moduleIndex, int portIndex)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    sendData(P_TXMODE_NORMAL, order);
}

void CDeviceImpl::setPortTxModeStrict(int moduleIndex, int portIndex)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    sendData(P_TXMODE_STRICUNIFORM, order);
}

void CDeviceImpl::setPortTxModeSequential(int moduleIndex, int portIndex)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    sendData(P_TXMODE_SEQUENTIAL, order);
}

bool CDeviceImpl::portRateFraction(int moduleIndex, int portIndex, float &fraction)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_RATEFRACTION_QUERY,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RATEFRACTION)
        {
            fraction = m_parser->text().toFloat() / 10000;
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

void CDeviceImpl::setPortRateFraction(int moduleIndex, int portIndex, float fraction)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.data.setNum(fraction * 10000);
    sendData(P_RATEFRACTION_SET, order);
}

bool CDeviceImpl::portPacketRate(int moduleIndex, int portIndex, int &packetRate)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_RATEPPS_QUERY,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RATEPPS)
        {
            packetRate = m_parser->text().toInt();
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

void CDeviceImpl::setPortPacketRate(int moduleIndex, int portIndex, int packetRate)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.data.setNum(packetRate);
    sendData(P_RATEPPS_SET, order);
}

bool CDeviceImpl::portBitRate(int moduleIndex, int portIndex, double &bitRate)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_RATEL2BPS_QUERY,order) && waitForRead())
    {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RATEL2BPS)
        {
            bitRate = m_parser->text().toDouble() / 1000000;
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}

void CDeviceImpl::setPortBitRate(int moduleIndex, int portIndex, double bitRate)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.data.setNum(bitRate * 1000000);
    sendData(P_RATEL2BPS_SET, order);
}

bool CDeviceImpl::portTxStatistics(int moduleIndex, int portIndex, int & bps,int & pps)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(PT_TOTAL, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_PT_TOTAL)
        {
            QStringList statisticsList = m_parser->text().split(" ");
            bps = statisticsList[0].toUInt();
            pps = statisticsList[1].toUInt();
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}



bool CDeviceImpl::streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }
    disconnectRecv();

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;
    if (sendData(PS_MODIFIERCOUNT, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_PS_MODIFIERCOUNT)
        {
            num = m_parser->text().toInt();
            connectRecv();
            return true;
        }
    }

    connectRecv();
    return false;
}


void CDeviceImpl::setStreamEnabled(int moduleIndex, int portIndex, int streamIndex, bool enabled)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;
    if (enabled) {
        sendData(PS_ENABLE, order);
    }
    else 
        sendData(PS_DISENABLE, order);
}


void CDeviceImpl::setStreamRate(int moduleIndex, int portIndex, int streamIndex, float percent)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(static_cast<int>(percent*10000));
    sendData(PS_RATEFRACTION_SET,order);                      // 设置流的发送速率
}

void CDeviceImpl::setStreamRate(int moduleIndex, int portIndex, int streamIndex, int packets)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(packets);
    sendData(PS_RATEPPS_SET,order);                      // 设置流的发送速率
}

bool CDeviceImpl::setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(index + 1);
    return sendData(PS_MODIFIERCOUNT_SET,order);                      // 设置跳变域个数
}

void CDeviceImpl::setStreamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int limitNum)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(limitNum);
    sendData(PS_PACKETLIMIT_SET,order);                    // 设置报文的帧数限制
}

void CDeviceImpl::setStreamModifierInc(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.modify.modifyIndex = modify.modifyIndex;
    order.modify.pos = modify.pos;
    order.modify.repeat = modify.repeat;

    sendData(PS_MODIFIER_INC,order);                    // 设置增加型跳变域
}

void CDeviceImpl::setStreamModifierDec(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.modify.modifyIndex = modify.modifyIndex;
    order.modify.pos = modify.pos;
    order.modify.repeat = modify.repeat;

    sendData(PS_MODIFIER_DEC,order);                    // 设置减少型跳变域
}

void CDeviceImpl::setStreamModifierRam(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.modify.modifyIndex = modify.modifyIndex;
    order.modify.pos = modify.pos;
    order.modify.repeat = modify.repeat;

    sendData(PS_MODIFIER_RAM,order);                    // 设置随机型跳变域
}

void CDeviceImpl::setStreamModifierRange(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.modify.modifyIndex = modify.modifyIndex;
    order.modify.minVal = modify.minVal;
    order.modify.step = modify.step;
    order.modify.maxVal = modify.maxVal;

    sendData(PS_MODIFIER_RANGE,order);                    // 设置跳变域的范围
}


bool CDeviceImpl::addStream(int moduleIndex, int portIndex, CStream *stream)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }
    
    // 创建流
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = stream->itemOfPort();                  // 此时第一条流还没加入到端口中

    if (_streamInit(order, stream) && _streamConfig(order, stream)) {
        return true;
    }
    return false;
}

bool CDeviceImpl::delStream(int moduleIndex, int portIndex, CStream *stream)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }
    /*disconnectRecv();*/

    // 删除流
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = stream->itemOfPort();
    return sendData(PS_DELETE, order);
}

bool CDeviceImpl::acquireChassisInfo(CChassis *chassis)
{
    bool rc = false;
    if (_requestChassisInfo()) {
        int mesc = -1;
        QString text;
        text.reserve(256);
        while(waitForRead(mesc)) {
            mesc = XenaManageSettings::waitForcontinueReadReadyTime();
            QString tmp;
            if (receiveData(tmp)) {
                text += tmp;
            }
        }
        _fillChassisInfo(text, chassis);
        rc = !m_bSocketExceptions;
    }
    
    if (rc)
    {
        for(int m = 0; m < m_moduleCount; m++) {
            CModule* module = new CModule();
            for(int p = 0; p < m_portsCount.at(m); p++) {
                CPort* port = new CPort();
                module->appendPort(port);
            }
            chassis->appendModule(module);
        }
    }

    return true;
}

bool CDeviceImpl::acquireOneModuleInfo(int moduleIndex, CModule *module)
{
    if (_requestOneModuleInfo(moduleIndex)) {
        int mesc = -1;
        QString text;
        text.reserve(256);
        while(waitForRead(mesc))
        {
            mesc = XenaManageSettings::waitForcontinueReadReadyTime();
            QString tmp;
            if (receiveData(tmp)) {
                text += tmp;
            }
        }
        _fillModuleInfo(text, module->chassis());
        return !m_bSocketExceptions;
    }

    return false;
}

bool CDeviceImpl::acquireAllModuleInfo(CChassis *chassis)
{
    for(int m = 0; m < m_moduleCount; m++) {
        _requestOneModuleInfo(m);
    }

    int mesc = -1;
    QString text;
    text.reserve(chassis->moduleCount()*256);
    while(waitForRead(mesc))
    {
        mesc = XenaManageSettings::waitForcontinueReadReadyTime();
        QString tmp;
        if (receiveData(tmp)) {
            text += tmp;
        }
    }
    _fillModuleInfo(text, chassis);
    return !m_bSocketExceptions;
}

bool CDeviceImpl::acquireOnePortInfo(int moduleIndex, int portIndex, CPort *port)
{
    if (_requestOnePortInfo(moduleIndex, portIndex)) {
        int mesc = -1;
        QString text;
        text.reserve(256);
        while(waitForRead(mesc))
        {
            mesc = XenaManageSettings::waitForcontinueReadReadyTime();
            QString tmp;
            if (receiveData(text)) {
                text += tmp;
            }
        }
        _fillPortInfo(text, port->module()->chassis());
        return !m_bSocketExceptions;
    }

    return false;
}

bool CDeviceImpl::acquireAllPortInfo(CChassis *chassis)
{
    for(int m = 0; m < m_moduleCount; m++) {
        for(int p = 0; p < m_portsCount.at(m); p++) {
            _requestOnePortInfo(m, p);
        }
    }

    int mesc = -1;
    QString text;
    text.reserve(chassis->moduleCount()*2048);
    while(waitForRead(mesc))
    {
        mesc = XenaManageSettings::waitForcontinueReadReadyTime();
        QString tmp;
        if (receiveData(tmp)) {
            text += tmp;
        }
    }
    _fillPortInfo(text, chassis);

    return !m_bSocketExceptions;
}

bool CDeviceImpl::acquireAllPortConfig(CChassis *chassis)
{
    for(int m = 0; m < m_moduleCount; m++) {
        for(int p = 0; p < m_portsCount.at(m); p++) {
            _requestOnePortConfig(m, p);
        }
    }

    int mesc = -1;
    QString text;
    text.reserve(chassis->moduleCount()*2048);
    while(waitForRead(mesc))
    {
        mesc = XenaManageSettings::waitForcontinueReadReadyTime();
        QString tmp;
        if (receiveData(tmp)) {
            text += tmp;
        }
    }
    _fillPortConfig(text, chassis);
    return !m_bSocketExceptions;
}

bool CDeviceImpl::acquireAllPortFullConfig(CChassis *chassis)
{
    for(int m = 0; m < m_moduleCount; m++) {
        for(int p = 0; p < m_portsCount.at(m); p++) {
            _requestOnePortFullConfig(m, p);
        }
    }

    int mesc = -1;
    QString text;
    text.reserve(chassis->moduleCount()*8192);
    while(waitForRead(mesc))
    {
        mesc = XenaManageSettings::waitForcontinueReadReadyTime();
        QString tmp;
        if (receiveData(tmp)) {
            text += tmp;
        }
    }

    _fillPortConfig(text, chassis);
    _fillPortFullConfig(text, chassis);
    _fillStreamInfo(text, chassis);
    _fillModifierInfo(text, chassis);
    return !m_bSocketExceptions;
}

bool CDeviceImpl::acquireStreamInfo(int moduleIndex, int portIndex, int streamIndex, CStream *stream)
{
    Q_ASSERT(false);

    return true;
}



bool CDeviceImpl::updateStream(int moduleIndex, int portIndex, CStream *stream)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }
    
    // 创建流
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = stream->itemOfPort();

    return _streamConfig(order, stream);
}

bool CDeviceImpl::updatePort(int moduleIndex, int portIndex, CPort* port)
{
    return true;
}

bool CDeviceImpl::updateModule(int moduleIndex, CModule* module)
{
    return true;
}


bool CDeviceImpl::sendData(E_ORDER type,S_ORDER & order)
{
    QByteArray data = m_cmdMgr.cmdToByteArray(type,order);
    qDebug() << "*******send:" << data;

#if DEBUG_CONTEXT
    writeDebugConetext(QString("*******send:") + data.data());
#endif

    if (-1 == m_pTcpSocket->write(data,data.size()))
    {
        return false;
    }
    return true;
}


bool CDeviceImpl::sendData(E_ORDERS types, S_ORDER &order)
{
    QByteArray data;
    Q_FOREACH(E_ORDER type, types) {
        data += m_cmdMgr.cmdToByteArray(type,order);
    }
    qDebug() << "*******send:" << data;

#if DEBUG_CONTEXT
    writeDebugConetext(QString("*******send:  ") + data.data());
#endif

    if (-1 == m_pTcpSocket->write(data,data.size()))
    {
        return false;
    }
    return true;
}

/*
 *  等待接收到数据
 */
bool CDeviceImpl::waitForRead(int mesc/* = -1*/)
{
    if (mesc == -1) {
        mesc = XenaManageSettings::waitForReadReadyTime();
    }
    return m_pTcpSocket->waitForReadyRead(mesc);
}

bool CDeviceImpl::receiveData(QString &text)
{
    text = m_pTcpSocket->readAll();
    if (!text.isEmpty()) {
        qDebug() << "receiveData:" << text;

#if DEBUG_CONTEXT
        writeDebugConetext(QString("******receiveData:  ") + text);
#endif

        return true;
    }
    return false;
}


bool CDeviceImpl::queryPortCounts()
{
    Q_ASSERT(m_bLogin);

    m_moduleCount = -1;
    m_portsCount.clear();

    if (sendData(C_PORTCOUNTS) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_C_PORT_COUNTS) {
            const QString &text = m_parser->text();
            QStringList splitText = text.split(' ');
            m_moduleCount = splitText.size();
            Q_FOREACH(const QString &str, splitText) {
                m_portsCount.push_back(str.toInt());
            }
            m_bQueryPortCount = true;
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::_streamInit(S_ORDER &order, CStream *stream)
{
    if (m_bSocketExceptions) {
        return false;
    }

    sendData(PS_CREATE, order);
    order.data = "-1";  
    sendData(PS_TPLDID,order);                                  // 设置流的ID  默认不使用最后20个字节

    //int portHeaderLength = stream->port()->maxHeaderLength();   // 获取此时端口的最大头长度
    order.data = QString::number(stream->const_data()->frameLength - 18);  // 18为以太网和VLAN的长度 剩余为segment的长度 每段segment 64 bytes
    sendData(PS_HEADERPROTOCOL,order);                          // 设置报文头的规约  以太网和VLAN + segment

    order.data = QString::number(stream->const_data()->frameLength + 4);
    sendData(PS_PACKETLENGTH_FIXED,order);                      // 设置报文的固定长度

    return true;
}

bool CDeviceImpl::_streamConfig(S_ORDER &order, CStream *stream)
{
    if (m_bSocketExceptions) {
        return false;
    }

    if (stream->isEnableTx()) 
        sendData(PS_ENABLE, order);                             // 使能流
    else
        sendData(PS_DISENABLE, order);                          // 禁用流
    if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
        order.data = QString::number(static_cast<int>(stream->txPercent()*10000));
        sendData(PS_RATEFRACTION_SET,order);                    // 设置发送速率
    }
    else {
        order.data = QString::number(stream->txFramesPerSecond());
        sendData(PS_RATEPPS_SET,order);                         // 设置发送速率
    }

    order.data.clear();
    for(int i = 0; i < stream->const_data()->frameLength;i++)
    {
        order.data += QString("%1").arg(stream->frames()[i], 2, 16,QChar('0'));
    }
    sendData(PS_PACKETHEADER,order);                            // 设置报文内容

    order.data = stream->streamName();
    sendData(PS_COMMENT,order);                                 // 设置流的描述内容

    return true;
}

bool CDeviceImpl::updateStreamHeader(int moduleIndex, int portIndex, int streamIndex, CStream *stream)
{
    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data = QString::number(stream->const_data()->frameLength - 18);  // 18为以太网和VLAN的长度 剩余为segment的长度 每段segment 64 bytes
    sendData(PS_HEADERPROTOCOL,order);                          // 设置报文头的规约  以太网和VLAN + segment

    order.data.clear();
    for(int i = 0; i < stream->const_data()->frameLength;i++)
    {
        order.data += QString("%1").arg(stream->frames()[i], 2, 16,QChar('0'));
    }
    sendData(PS_PACKETHEADER,order);                            // 更新报文内容

    return true;
}


bool CDeviceImpl::_requestChassisInfo()
{
    E_ORDERS cmds;
    cmds << C_INFO << C_CONFIG;

    return sendData(cmds);
}

bool CDeviceImpl::_fillChassisInfo(const QString& cmdText, CChassis *chassis)
{
    QStringList cmd_arr = cmdText.split('\n');
    Q_FOREACH(const QString &cmd, cmd_arr)
    {
        m_parser->parse(cmd);
        switch(m_parser->result()) {
        // C_INFO 
        case CXenaParser::RL_C_RESERVATION:
            if (m_parser->text().contains("reserved", Qt::CaseInsensitive))
                chassis->setReserved(true);
            else
                chassis->setReserved(false);
            break;
        case CXenaParser::RL_C_RESERVEDBY:
            chassis->setReservedBy(m_parser->text());
            break;
        case CXenaParser::RL_C_PORT_COUNTS:
            {
                QStringList splitText = m_parser->text().split(' ');
                m_moduleCount = splitText.size();
                Q_FOREACH(const QString &str, splitText) {
                    m_portsCount.push_back(str.toInt());
                }
                m_bQueryPortCount = true;
            }
            break;
        case CXenaParser::RL_C_SERIALNO:
            chassis->setChassisSerialNo(m_parser->text());
            break;
        //C_CONFIG
        case CXenaParser::RL_C_NAME:
            chassis->setChassisName(m_parser->text());
            break;
        default:
            break;
        }
    }


    return true;
}

bool CDeviceImpl::_requestOneModuleInfo(int moduleIndex)
{
    E_ORDERS cmds;
    cmds << M_INFO /*<< M_CONFIG*/;

    S_ORDER order;
    order.moduleIndex = moduleIndex;

    return sendData(cmds, order);
}

bool CDeviceImpl::_fillModuleInfo(const QString& cmdText, CChassis *chassis)
{
    QStringList cmd_arr = cmdText.split('\n');
    Q_FOREACH(const QString &cmd, cmd_arr)
    {
        m_parser->parse(cmd);
        int m = m_parser->moduleIndex();
        if (m >= 0 && m < chassis->moduleCount())
        {
            switch(m_parser->result()) {
            //M_INFO
            case CXenaParser::RL_M_RESERVATION:
                if (m_parser->text().contains("reserved", Qt::CaseInsensitive))
                    chassis->module(m)->setReserved(true);
                else
                    chassis->module(m)->setReserved(false);
                break;
            case CXenaParser::RL_M_RESERVEDBY:
                chassis->module(m)->setReservedBy(m_parser->text());
                break;
            case CXenaParser::RL_M_MODEL:
                chassis->module(m)->setName(m_parser->text());
                chassis->module(m)->setBadModule(false);
                break;
            default:
                break;
            }
        }
    }

    return true;
}

bool CDeviceImpl::_requestOnePortInfo(int moduleIndex, int portIndex)
{
    E_ORDERS cmds;
    cmds << P_INFO << P_RECEIVESYNC;

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    return sendData(cmds, order);
}

bool CDeviceImpl::_requestOnePortConfig(int moduleIndex, int portIndex)
{
    E_ORDERS cmds;
    cmds << P_MAXHEADERLENGTH_QUERY << P_TXTIMELIMIT_QUERY << P_TXMODE_QUERY 
         << P_RATEFRACTION_QUERY << P_RATEPPS_QUERY << P_RATEL2BPS_QUERY;

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    return sendData(cmds, order);
}

bool CDeviceImpl::_requestOnePortFullConfig(int moduleIndex,int portIndex)
{
    E_ORDERS cmds;
    cmds << P_FULLCONIFG << P_RATEFRACTION_QUERY << P_RATEPPS_QUERY << P_RATEL2BPS_QUERY;

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    return sendData(cmds, order);
}

bool CDeviceImpl::_fillPortInfo(const QString& cmdText, CChassis *chassis)
{
    QStringList cmd_arr = cmdText.split('\n');
    Q_FOREACH(const QString &cmd, cmd_arr)
    {
        m_parser->parse(cmd);
        int m = m_parser->moduleIndex();
        if (m >= 0 && m < chassis->moduleCount()) {
            int p = m_parser->portIndex();
            CModule* module = chassis->module(m);
            if (p >= 0 && p < module->portCount()) {
                switch(m_parser->result()) {
                //P_INFO
                case CXenaParser::RL_P_RESERVATION:
                    if (m_parser->text().contains("reserved", Qt::CaseInsensitive))
                        module->port(p)->setReserved(true);
                    else
                        module->port(p)->setReserved(false);
                    break;
                case CXenaParser::RL_P_RESERVEDBY:
                    module->port(p)->setReservedBy(m_parser->text());
                    break;
                case CXenaParser::RL_P_INTERFACE:
                    module->port(p)->setPortName(m_parser->text());
                    module->port(p)->setBadPort(false);
                    break;
                case CXenaParser::RL_P_SPEED:
                    module->port(p)->setRealSpeed(m_parser->text().toUInt());
                    break;
                case CXenaParser::RL_P_TRAFFIC:
                    if (m_parser->text().contains("ON", Qt::CaseInsensitive))
                        module->port(p)->setActived(true);
                    else
                        module->port(p)->setActived(false);
                    break;
                    //P_CONFIG
                case CXenaParser::RL_P_RECEIVESYNC:
                    if (m_parser->text().contains("IN_SYNC", Qt::CaseInsensitive))
                        module->port(p)->setLinked(true);
                    else
                        module->port(p)->setLinked(false);
                    break;
                default:
                    if (m_parser->result() < 0) {
                        module->port(p)->setLinked(false);
                        module->port(p)->setActived(false);
                    }
                    break;
                }
            }
        }
    }

    return true;
}

bool CDeviceImpl::_fillPortConfig(const QString & cmdText, CChassis *chassis)
{
    QStringList cmd_arr = cmdText.split('\n');
    Q_FOREACH(const QString &cmd, cmd_arr)
    {
        m_parser->parse(cmd);
        int m = m_parser->moduleIndex();
        if (m >= 0 && m < chassis->moduleCount()) {
            int p = m_parser->portIndex();
            CModule* module = chassis->module(m);
            if (p >= 0 && p < module->portCount()) {
                switch(m_parser->result()) {
                    case CXenaParser::RL_P_MAXHEADERLENGTH:
                        {
                            int maxLen = m_parser->text().toInt();
                            module->port(p)->setMaxHeaderLength(maxLen);
                            break;
                        }
                    case CXenaParser::RL_P_TXTIMELIMIT:
                        {
                            int limitMs = m_parser->text().left(m_parser->text().size() - 3).toInt();
                            module->port(p)->setTxTimeLimit(limitMs);
                            break;
                        }
                    case CXenaParser::RL_P_TXMODE:
                        {
                            QString mode = m_parser->text();
                            module->port(p)->setTxMode(mode);
                            break;
                        }
                    case CXenaParser::RL_P_RATEFRACTION:
                        {
                            float fraction = m_parser->text().toFloat() / 10000;
                            module->port(p)->setRateFraction(fraction);
                            break;
                        }
                    case CXenaParser::RL_P_RATEPPS:
                        {
                            int packetRate = m_parser->text().toInt();
                            module->port(p)->setPacketRate(packetRate);
                            break;
                        }
                    case CXenaParser::RL_P_RATEL2BPS:
                        {
                            double bitRate = m_parser->text().toDouble() / 1000000;
                            module->port(p)->setBitRate(bitRate);
                            break;
                        }
                    default:
                        break;
                }
            }
        }
    }

    return true;
}

bool CDeviceImpl::_fillPortFullConfig(const QString & cmdText, CChassis *chassis)
{
    QStringList cmd_arr = cmdText.split('\n');
    Q_FOREACH(const QString &cmd, cmd_arr)
    {
        m_parser->parse(cmd);
        int m = m_parser->moduleIndex();
        if (m >= 0 && m < chassis->moduleCount()) {
            int p = m_parser->portIndex();
            CModule* module = chassis->module(m);
            if (p >= 0 && p < module->portCount()) {
                switch(m_parser->result()) {
                // P_FULLCONFIG
                case CXenaParser::RL_PS_STREAM_NUM:
                    {
                        if (!m_parser->text().isEmpty()){
                            QString str = m_parser->text();
                            QStringList strList = m_parser->text().split(" ");
                            for (int i = 0;i < strList.size();i++){
                                CStream *stream = new CStream;
                                stream->setItemOfPort(strList[i].toInt());
                                module->port(p)->appendStream(stream);
                            }
                        }
                        break;
                    }
                default:
                    break;
                }
            }
        }
    }

    return true;
}

bool CDeviceImpl::_fillStreamInfo(const QString & cmdText, CChassis *chassis)
{
    QStringList cmd_arr = cmdText.split('\n');
    Q_FOREACH(const QString &cmd, cmd_arr)
    {
        m_parser->parse(cmd);
        int m = m_parser->moduleIndex();
        if (m >= 0 && m < chassis->moduleCount()) {
            int p = m_parser->portIndex();
            CModule* module = chassis->module(m);
            if (p >= 0 && p < module->portCount()) {
                int s = m_parser->streamIndex();
                CPort *port = chassis->module(m)->port(p);
                for (int i = 0;i < port->streamCount();i++)
                {
                    if (s == port->stream(i)->itemOfPort()){
                        switch(m_parser->result()) {
                        case CXenaParser::RL_PS_COMMENT:
                            port->stream(i)->setStreamName(m_parser->text());
                            break;
                        case CXenaParser::RL_PS_MODIFIERCOUNT: //??????
                            {
                                for (int j = 0;j < m_parser->text().toInt();j++)
                                {
                                    CModifierDialog::ModifierData *modifierData = new CModifierDialog::ModifierData;
                                    port->stream(i)->appendModifierData(modifierData);
                                }
                                break;
                            }
                        case CXenaParser::RL_PS_PACKETLIMIT:
                            if (m_parser->text().toInt() >= 0){
                                port->stream(i)->setStrategy(m_parser->text().toInt());
                            }
                            else{
                                port->stream(i)->setStrategy(0);
                            }
                            break;
                        case CXenaParser::RL_PS_RATEFRACTION:
                            port->stream(i)->setTxPercent(m_parser->text().toFloat() / 10000);
                            break;
                        case CXenaParser::RL_PS_ENABLE:
                            if (m_parser->text().contains("ON",Qt::CaseInsensitive)){
                                port->stream(i)->enableTx(true);
                            }else{
                                port->stream(i)->enableTx(false);
                            }
                            break;
                        case CXenaParser::RL_PS_TPLDID: // 设置的都为-1
                            break;
                        case CXenaParser::RL_PS_PACKETHEADER:
                            {
                                QString ethData = m_parser->text().mid(2);
                                int streamLength = ethData.size() / 2;
                                unsigned char * data = new unsigned char[streamLength];
                                for (int j = 0;j < streamLength;j++)
                                {
                                    data[j] = ethData.mid(j*2, 2).toUInt(0, 16);
                                }
                                ethFrameToImportData(data,streamLength,port->stream(i)->data());
                                delete[] data;
                                break;
                            }
                        case CXenaParser::RL_PS_HEADERPROTOCOL:
                            break;
                        case CXenaParser::RL_PS_PACKETLENGTH:
                            {
                                break;
                            }
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool CDeviceImpl::_fillModifierInfo(const QString & cmdText, CChassis *chassis)
{
    QStringList cmd_arr = cmdText.split('\n');
    Q_FOREACH(const QString &cmd, cmd_arr)
    {
        m_parser->parse(cmd);
        int m = m_parser->moduleIndex();
        if (m >= 0 && m < chassis->moduleCount()) {
            int p = m_parser->portIndex();
            CModule* module = chassis->module(m);
            if (p >= 0 && p < module->portCount()) {
                int s = m_parser->streamIndex();
                CPort *port = chassis->module(m)->port(p);
                for (int i = 0;i < port->streamCount();i++)
                {
                    if (s == port->stream(i)->itemOfPort()){
                        int mo = m_parser->modifierIndex();
                        CStream *stream = port->stream(i);
                        if (mo >= 0 && mo < stream->modifierDataCount()){
                            switch(m_parser->result()) {
                            case CXenaParser::RL_PS_MODIFIER:
                                {
                                    QStringList strList = m_parser->text().split(" ");
                                    stream->modifierData(mo)->setPos(strList[0].toInt());
                                    stream->modifierData(mo)->setBitsNum(strList[1]);
                                    stream->modifierData(mo)->setModifyType(strList[2]);
                                    stream->modifierData(mo)->setRepeatCnt(strList[3].toInt());
                                    break;
                                }
                            case CXenaParser::RL_PS_MODIFIERRANGE:
                                {
                                    QStringList strList = m_parser->text().split(" ");
                                    stream->modifierData(mo)->setMinVal(strList[0].toInt());
                                    stream->modifierData(mo)->setMaxVal(strList[1].toInt());
                                    break;
                                }
                            default:
                                break;
                            }
                        }
                    }
                }
                
            }
        }
    }

    return true;
}

bool CDeviceImpl::startMonitorChassisStatus(CChassis* chassis)
{
    m_monitorChassis = chassis;
    connectRecv();
    m_pollCnt = 0;
    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, SIGNAL(timeout()), this, SLOT(pollChassis()));
    m_pollTimer->start(1000);
    return true;
}

bool CDeviceImpl::stopMonitorChassisStatus()
{
    if(m_pollTimer) {
        m_pollTimer->stop();
        m_pollTimer->deleteLater();
        m_pollTimer = NULL;
    }

    return true;
}


void CDeviceImpl::receiveMsg()
{
    QString text;
    if (receiveData(text)) {
        _fillPortInfo(text, m_monitorChassis);
    }
}

void CDeviceImpl::pollChassis()
{
    if (m_monitorChassis) {
        for(int m = 0; m < m_monitorChassis->moduleCount(); m++) {
            if (m == m_pollCnt) {
                CModule* module = m_monitorChassis->module(m);
                for(int p = 0; p < module->portCount(); p++) {
                    _requestOnePortInfo(m, p);
                }
                m_pollCnt++;
                if (m_pollCnt == m_monitorChassis->moduleCount())
                    m_pollCnt = 0;
                break;
            }
        }
    }
}

void CDeviceImpl::connectRecv()
{
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(receiveMsg()));
}

void CDeviceImpl::disconnectRecv()
{
    disconnect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(receiveMsg()));
}