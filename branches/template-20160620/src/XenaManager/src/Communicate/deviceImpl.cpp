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
: IDevice(parent)
, m_bLogin(false)
, m_parser(NULL)
, m_bQueryPortCount(false)
, m_bSocketExceptions(false)
{
    m_pTcpSocket = new QTcpSocket(this);
    m_parser = new CXenaParser();
}

CDeviceImpl::~CDeviceImpl()
{
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
        QMessageBox::warning(0,tr("警告"),tr("网络链接不成功，请检查网络"),QString(tr("确定")));
        m_bSocketExceptions = true;
        break;
    case QAbstractSocket::RemoteHostClosedError:
        QMessageBox::warning(0,tr("警告"),tr("网络链接断开，请检查网络"),QString(tr("确定")));
        m_bSocketExceptions = true;
        qApp->exit(0);
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(0,tr("警告"),tr("网络链接超时，请重新链接"),QString(tr("确定")));
        m_bSocketExceptions = true;
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::warning(0,tr("警告"),tr("网络链接失败，请重新输入IP和端口号"),QString(tr("确定")));
        m_bSocketExceptions = true;
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::warning(0,tr("警告"),tr("网络链接失败，请重新输入IP和端口号"),QString(tr("确定")));
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
    connect(m_pTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    m_pTcpSocket->connectToHost(ip,port);
    if(!m_pTcpSocket->waitForConnected(XenaManageSettings::waitConnectTime())) {
        qDebug() << "disConnected!";
        return false;
    }
    m_bSocketExceptions = false;

    m_cmdMgr.setUserName(userName);
    m_cmdMgr.setPassword(pwd);

    this->receiveData();
    if (this->sendData(C_LOGON) && this->waitForRead()) {
        QString text;
        this->receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_OK) {
            if (this->sendData(C_OWNER) && this->waitForRead()) {
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

    this->sendData(C_KEEPALIVE);
    if (this->waitForRead()) {
        this->receiveData();
        return true;
    }

    return false;
}

/*
 *  机箱名字
 */
bool CDeviceImpl::chassisName(QString &name, QString &desc)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    this->sendData(C_NAME);
    if (this->waitForRead()) {
        QString text;
        this->receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_C_NAME) {
            name = m_parser->text();
            name.remove('\"');
			name.remove("\n");
            return true;
        }
    }

    return false;
}

/*
 *  机箱序列号
 */
bool CDeviceImpl::chassisSerialNo(QString &serialNo)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    this->sendData(C_SERIALNO);
    if (this->waitForRead()) {
        QString text;
        this->receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_C_SERIALNO) {
            serialNo = m_parser->text();
            serialNo.remove('\"');
            return true;
        }
    }

    return false;
}

/*
 *  机箱使用者
 *  
 *  @Return:
 *      true：成功获取机箱使用者状态，如果未有使用者占用机箱，owner = ""
 *      false:通讯异常
 */
bool CDeviceImpl::chassisReservedOwner(QString &owner)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    if (sendData(C_RESERVATION_QUERY) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_C_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(C_RESERVEDBY) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_C_RESERVEDBY) {
                    owner = m_parser->text();
                    owner.remove('\"');
                    owner.remove("\n");
                    return true;
                }
            }
        }
    }

    owner = "";
    return false;
}

bool CDeviceImpl::reserveChassis()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    if (sendData(C_RESERVATION_QUERY) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_C_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            return false;
        }

        if (sendData(C_RESERVATION_RESERVE) && waitForRead()) {
            receiveData(text);
            m_parser->parse(text);
            if (m_parser->result() == CXenaParser::RL_OK) {
                return true;
            }
        }
    }

    return false;
}

bool CDeviceImpl::releaseChassis()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    if (sendData(C_RESERVATION_QUERY) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_C_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(C_RESERVATION_RELEASE) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_OK) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool CDeviceImpl::relinquishChassis()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    if (sendData(C_RESERVATION_QUERY) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_C_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(C_RESERVATION_RELINQUISH) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_OK) {
                    return true;
                }
            }
        }
    }

    return false;
}



int  CDeviceImpl::moduleCount()
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    if (!m_bQueryPortCount) {
        queryPortCounts();
    }
    return m_moduleCount;
}

bool CDeviceImpl::moduleName(int modelIndex, QString &name, QString &desc)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = modelIndex;
    if (sendData(M_MODEL, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_M_MODEL) {
            name = m_parser->text();
            name.remove('\"');
			name.remove("\n");
            return true;
        }
    }

    return false;
}


int CDeviceImpl::allModuleName(QStringList& nameList)
{
    QMutexLocker _lock(&m_mutex);

    if (m_bSocketExceptions) {
        return false;
    }

    Q_ASSERT(m_bLogin && m_bQueryPortCount);
    for(int i = 0; i < m_moduleCount; i++) {
        nameList.push_back("BAD");
    }
    sendData(M_MODULE_COUNTS);
    QString text;
    while(waitForRead()) {
        if (m_bSocketExceptions) {
            break;
        }
        receiveData(text);
        QStringList listText = text.split('\n');
        Q_FOREACH(const QString &str, listText) {
            m_parser->parse(str);
            if (m_parser->result() == CXenaParser::RL_M_MODEL) {
                nameList[m_parser->moduleIndex()] = m_parser->text();
                if (m_parser->moduleIndex() == m_moduleCount-1) {
                    return m_moduleCount;
                }
            }
        }
    }
    return m_moduleCount;
}

/*
 *  板卡使用者
 *  
 *  @Return:
 *      true：成功获取机箱使用者状态，如果未有使用者占用板卡，owner = ""
 *      false:通讯异常
 */
bool CDeviceImpl::moduleReservedOwner(int modelIndex, QString &owner)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = modelIndex;
    if (sendData(M_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_M_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(M_RESERVEDBY, order) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_M_RESERVEDBY) {
                    owner = m_parser->text();
                    owner.remove('\"');
                    owner.remove("\n");
                    return true;
                }
            }
        }
    }

    owner = "";
    return false;
}

bool CDeviceImpl::reserveModule(int moduleIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    if (sendData(M_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_M_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            return false;
        }

        if (sendData(M_RESERVATION_RESERVE, order) && waitForRead()) {
            receiveData(text);
            m_parser->parse(text);
            if (m_parser->result() == CXenaParser::RL_OK) {
                return true;
            }
        }
    }

    return false;
}

bool CDeviceImpl::releaseModule(int moduleIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    if (sendData(M_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_M_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(M_RESERVATION_RELEASE, order) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_OK) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool CDeviceImpl::relinquishModule(int moduleIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    if (sendData(M_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_M_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(M_RESERVATION_RELINQUISH, order) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_OK) {
                    return true;
                }
            }
        }
    }

    return false;
}



int  CDeviceImpl::portCount(int moduleIndex)
{
    QMutexLocker _lock(&m_mutex);

    if (m_bSocketExceptions) {
        return false;
    }

    if (!m_bQueryPortCount) {
        queryPortCounts();
    }

    if (moduleIndex < m_portsCount.size()) {
        return m_portsCount.at(moduleIndex);
    }

    return -1;
}

bool CDeviceImpl::portName(int moduleIndex, int portIndex, QString &name, QString &desc)
{
    QMutexLocker _lock(&m_mutex);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;

    if (sendData(P_INTERFACE, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_INTERFACE) {
            name = m_parser->text();
            name.remove('\"');
            name.remove('\n');
            return true;
        }
    }

    return false;
}

/*
 *  端口使用者
 *  
 *  @Return:
 *      true：成功获取机箱使用者状态，如果未有使用者占用板卡，owner = ""
 *      false:通讯异常
 */
bool CDeviceImpl::portReservedOwner(int modelIndex, int portIndex, QString &owner)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = modelIndex;
    order.portIndex = portIndex;
    if (sendData(P_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(P_RESERVEDBY, order) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_P_RESERVEDBY) {
                    owner = m_parser->text();
                    owner.remove('\"');
                    owner.remove("\n");
                    return true;
                }
            }
        }
    }

    owner = "";
    return false;
}

bool CDeviceImpl::reservePort(int moduleIndex, int portIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(P_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            return false;
        }

        if (sendData(P_RESERVATION_RESERVE, order) && waitForRead()) {
            receiveData(text);
            m_parser->parse(text);
            if (m_parser->result() == CXenaParser::RL_OK) {
                return true;
            }
        }
    }

    return false;
}

bool CDeviceImpl::releasePort(int moduleIndex,int portIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(P_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(P_RESERVATION_RELEASE, order) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_OK) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool CDeviceImpl::relinquishPort(int moduleIndex,int portIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(P_RESERVATION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RESERVATION && m_parser->text().contains("reserved", Qt::CaseInsensitive))
        {
            if (sendData(P_RESERVATION_RELINQUISH, order) && waitForRead()) {
                receiveData(text);
                m_parser->parse(text);
                if (m_parser->result() == CXenaParser::RL_OK) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool CDeviceImpl::portCommunicationStatus(int moduleIndex, int portIndex, bool &linked, bool &actived)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    int rc = 0;
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    linked = false;
    actived = false;
    if (sendData(P_RECEIVESYNC, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_RECEIVESYNC && m_parser->text().contains("IN_SYNC", Qt::CaseInsensitive))
        {
            linked = true;
        }
        rc |= 1;
    }
    if (sendData(P_TRAFFIC_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_TRAFFIC && m_parser->text().contains("ON", Qt::CaseInsensitive))
        {
            actived = true;
        }
        rc |= 2;
    }

    return (rc==3) ? true : false;
}

bool CDeviceImpl::portTPLDModel(int moduleIndex, int portIndex, QString &model)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(P_TPLDMODE_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_TPLDMODE)
        {
            model = m_parser->text();
            model.remove('\"');
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::portCheckSum(int moduleIndex, int portIndex, bool &enable)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    enable = false;
    if (sendData(P_CHECKSUM_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_CHECKSUM && m_parser->text().contains("ON", Qt::CaseInsensitive))
        {
            enable = true;
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::portMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(P_MAXHEADERLENGTH_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_MAXHEADERLENGTH)
        {
            maxLen = m_parser->text().toUInt();
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::portTimeLimit(int moduleIndex, int portIndex, int &limitMS)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(P_TXTIMELIMIT_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_TXTIMELIMIT)
        {
            limitMS = m_parser->text().toUInt() / 1000;
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::portRealSpeed(int moduleIndex, int portIndex, int &speed_mbps)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(P_SPEED, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_SPEED)
        {
            speed_mbps = m_parser->text().toUInt();
            return true;
        }
    }

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

    waitForRead();
    receiveData();
}

void CDeviceImpl::setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.data.setNum(maxLen);
    sendData(P_MAXHEADERLENGTH_SET, order);

    waitForRead();
    receiveData();
}
void CDeviceImpl::setPortTimeLimit(int moduleIndex, int portIndex, int limitMS)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.data.setNum(limitMS * 1000);
    sendData(P_TXTIMELIMIT_SET, order);

    waitForRead();
    receiveData();
}

void CDeviceImpl::setPortRealSpeed(int moduleIndex, int portIndex, int speed_mbps)
{
    Q_ASSERT(false);
}

bool CDeviceImpl::portTxStatistics(int moduleIndex, int portIndex, int & bps,int & pps)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

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
            return true;
        }
    }

    return false;
}



bool CDeviceImpl::streamCount(int moduleIndex, int portIndex, int &streamIndex)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(false);   //需要在scriptcommand.h添加解析代码
    return 0;
}

bool CDeviceImpl::streamComment(int moduleIndex, int portIndex, int streamIndex, QString & comment)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;
    if (sendData(P_COMMENT, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_P_COMMENT)
        {
            comment = m_parser->text();
            comment.remove('\"');
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::streamEnabled(int moduleIndex, int portIndex, int streamIndex, bool &enabled)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;
    enabled = false;
    if (sendData(PS_ENABLE_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_PS_ENABLE && m_parser->text().contains("ON", Qt::CaseInsensitive))
        {
            enabled = true;
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

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
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::streamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int &limitNum)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;
    if (sendData(PS_PACKETLIMIT_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_PS_PACKETLIMIT)
        {
            limitNum = m_parser->text().toInt();
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::streamRate(int moduleIndex, int portIndex, int streamIndex, float &percent)   // 获取流的速率(百分比)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;
    if (sendData(PS_RATEFRACTION_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_PS_RATEFRACTION)
        {
            percent = m_parser->text().toFloat();
            return true;
        }
    }

    return false;
}

bool CDeviceImpl::streamRate(int moduleIndex, int portIndex, int streamIndex, int &packets)     //获取流的速率(帧每秒)
{
    QMutexLocker _lock(&m_mutex);
    Q_ASSERT(m_bLogin);

    if (m_bSocketExceptions) {
        return false;
    }

    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;
    if (sendData(PS_RATEPPS_QUERY, order) && waitForRead()) {
        QString text;
        receiveData(text);
        m_parser->parse(text);
        if (m_parser->result() == CXenaParser::RL_PS_RATEPPS)
        {
            packets = m_parser->text().toInt();
            return true;
        }
    }

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
    waitForRead();
    receiveData();
}


void CDeviceImpl::setStreamRate(int moduleIndex, int portIndex, int streamIndex, float percent)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(static_cast<int>(percent*10000));
    sendData(PS_RATEFRACTION_SET,order);                      // 设置流的发送速率

    waitForRead();
    receiveData();
}

void CDeviceImpl::setStreamRate(int moduleIndex, int portIndex, int streamIndex, int packets)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(packets);
    sendData(PS_RATEPPS_SET,order);                      // 设置流的发送速率

    waitForRead();
    receiveData();
}

void CDeviceImpl::setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(index + 1);
    sendData(PS_MODIFIERCOUNT_SET,order);                      // 设置跳变域个数

    waitForRead();
    receiveData();
}

void CDeviceImpl::setStreamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int limitNum)
{
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = streamIndex;

    order.data.setNum(limitNum);
    sendData(PS_PACKETLIMIT_SET,order);                    // 设置报文的帧数限制

    waitForRead();
    receiveData();
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

    waitForRead();
    receiveData();
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

    waitForRead();
    receiveData();
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

    waitForRead();
    receiveData();
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

    waitForRead();
    receiveData();
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
    order.streamindex = stream->port()->streamCount();                  // 此时第一条流还没加入到端口中

    if (_streamInit(order, stream) && _streamConfig(order, stream)) {
        if (waitForRead() && receiveData()) {
            return true;
        }
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

    // 创建流
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    order.streamindex = stream->port()->findStream(stream);
    
    sendData(PS_DELETE,order);
    if (waitForRead() && receiveData()) {
        return true;
    }

    return false;
}

bool CDeviceImpl::acquireChassisInfo(CChassis *chassis)
{
    //M_MODULE_COUNTS
    E_ORDERS cmd_chassis;
    cmd_chassis << C_NAME << C_SERIALNO << C_PORTCOUNTS;
    int flag = 0x07;
    if (sendData(cmd_chassis)) {
        while(flag && waitForRead()) {
            if (m_bSocketExceptions) {
                break;
            }
            QString text;
            if (receiveData(text)) {
                QStringList cmd_arr = text.split('\n');
                Q_FOREACH(const QString &cmd, cmd_arr)
                {
                    m_parser->parse(cmd);
                    switch(m_parser->result()) {
                    case CXenaParser::RL_C_NAME:
                        if (flag&0x01) {
                            text = m_parser->text();
                            chassis->setChassisName(text.remove('\"'));
                            flag &= ~(1<<0);
                        }
                        break;
                    case CXenaParser::RL_C_SERIALNO:
                        if (flag&0x02) {
                            text = m_parser->text();
                            chassis->setChassisSerialNo(text.remove('\"'));
                            flag &= ~(1<<1);
                        }
                        break;
                    case CXenaParser::RL_C_PORT_COUNTS:
                        {
                            QStringList splitText = m_parser->text().split(' ');
                            m_moduleCount = splitText.size();
                            Q_FOREACH(const QString &str, splitText) {
                                m_portsCount.push_back(str.toInt());
                            }
                            m_bQueryPortCount = true;
                            flag &= ~(1<<2);
                        }
                        break;
                    default:
                        if (m_parser->result() < 0) {
                            flag = 0;
                        }
                        break;
                    }
                }
            }
        }
    }

    if (!flag) {
        QStringList moduleName_;
        allModuleName(moduleName_);
        for(int m = 0; m < m_moduleCount; m++) {
            CModule* module = new CModule();
            module->setName(moduleName_.at(m));
            for(int p = 0; p < m_portsCount.at(m); p++) {
                CPort* port = new CPort();
                acquirePortInfo(m, p, port);
                module->appendPort(port);
            }
            chassis->appendModule(module);
        }
        
    }

    return !flag;
}

bool CDeviceImpl::acquireModuleInfo(int moduleIndex, CModule *module)
{
    if (m_bSocketExceptions) {
        return false;
    }
    Q_ASSERT(m_bQueryPortCount);

    QString name;
    moduleName(moduleIndex, name);
    module->setName(name);
    for(int p = 0; p < m_portsCount.at(moduleIndex); p++) {
        CPort* port = new CPort();
        acquirePortInfo(moduleIndex, p, port);
        module->appendPort(port);
    }

    return true;
}

bool CDeviceImpl::acquirePortInfo(int moduleIndex, int portIndex, CPort *port)
{
    E_ORDERS cmd_port;
    cmd_port << P_INTERFACE << P_RECEIVESYNC << P_TRAFFIC_QUERY  << P_RESERVATION_QUERY << P_RESERVEDBY;
    int flag = 0x01F;
    S_ORDER order;
    order.moduleIndex = moduleIndex;
    order.portIndex = portIndex;
    if (sendData(cmd_port, order)) {
        while(flag && waitForRead()) {
            if (m_bSocketExceptions) {
                break;
            }
            QString text;
            if (receiveData(text)) {
                QStringList cmd_arr = text.split('\n');
                Q_FOREACH(const QString &cmd, cmd_arr)
                {
                    m_parser->parse(cmd);
                    switch(m_parser->result()) {
                    case CXenaParser::RL_P_INTERFACE:
                        if (flag&(1<<0)) {
                            text = m_parser->text();
                            port->setPortName(text.remove('\"'));
                            flag &= ~(1<<0);
                        }
                        break;
                    case CXenaParser::RL_P_RECEIVESYNC:
                        if (flag&(1<<1)) {
                            if (m_parser->text().contains("IN_SYNC", Qt::CaseInsensitive))
                                port->setLinked(true);
                            else
                                port->setLinked(false);
                            flag &= ~(1<<1);
                        }
                        break;
                    case CXenaParser::RL_P_TRAFFIC:
                        if (flag&(1<<2)) {
                            if (m_parser->text().contains("ON", Qt::CaseInsensitive))
                                port->setActived(true);
                            else
                                port->setActived(false);
                            flag &= ~(1<<2);
                        }
                        break;
                    case CXenaParser::RL_P_RESERVATION:
                        if (flag&(1<<3)) {
                            if (m_parser->text().contains("reserved", Qt::CaseInsensitive))
                                port->setReserved(true);
                            else
                                port->setReserved(false);
                            flag &= ~(1<<3);
                        }
                        break;
                    case CXenaParser::RL_P_RESERVEDBY:
                        if (flag&(1<<4)) {
                            port->setReservedBy(m_parser->text());
                            flag &= ~(1<<4);
                        }
                        break;
                    case CXenaParser::RL_ERROR_BAD_PORT:
                        port->setLinked(false);
                        port->setActived(false);
                        flag = 0;
                        break;
                    default:
                        if (m_parser->result() < 0) {
                            port->setLinked(false);
                            port->setActived(false);
                            flag = 0;
                        }
                        break;
                    }
                }
            }
        }
    }

    return !flag;
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

    if (_streamConfig(order, stream)) {
        if (waitForRead() && receiveData()) {
            return true;
        }
    }
    return false;
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
    if (-1 == m_pTcpSocket->write(data,data.size()))
    {
        return false;
    }
    return true;
}

/*
 *  等待接收到数据
 */
bool CDeviceImpl::waitForRead()
{
    return m_pTcpSocket->waitForReadyRead(XenaManageSettings::waitForReadReadyTime());
}

bool CDeviceImpl::receiveData(QString &text)
{
    text = m_pTcpSocket->readAll();
    if (!text.isEmpty()) {
        qDebug() << "receiveData:" << text;
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

    sendData(PS_CREATE, order);                                 // 创建流
    order.data = "-1";  
    sendData(PS_TPLDID,order);                                  // 设置流的ID  默认不使用最后20个字节

    //int portHeaderLength = stream->port()->maxHeaderLength();   // 获取此时端口的最大头长度
    order.data = QString::number(stream->streamLength() - 18);  // 18为以太网和VLAN的长度 剩余为segment的长度 每段segment 64 bytes
    sendData(PS_HEADERPROTOCOL,order);                          // 设置报文头的规约  以太网和VLAN + segment

    order.data = QString::number(stream->streamLength() + 4);
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
    for(int i = 0; i < stream->streamLength();i++)
    {
        order.data += QString("%1").arg(stream->frames()[i], 2, 16,QChar('0'));
    }
    sendData(PS_PACKETHEADER,order);                            // 设置报文内容

    return true;
}

