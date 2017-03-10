#pragma once

#include "device.h"

#include <QtNetwork/QTcpSocket>
#include <QList>
#include <QMutex>
#include <QThread>
#include <QTimer>

class CXenaParser;

class CDeviceImpl : public QObject, public IDevice
{
    Q_OBJECT
public:
    CDeviceImpl(QObject* parent = 0) ;
    virtual ~CDeviceImpl();

    virtual bool login(const QString &ip, int port, const QString &userName, const QString &pwd);
    virtual bool logout();
    
    bool isLogin() {return m_bLogin;}

    /*
     *  保持机箱一直处于在线状态
     */
    virtual bool keepAlive();

    virtual bool reserveChassis();
    virtual bool releaseChassis();
    virtual bool relinquishChassis();                                   // 注销机箱


    virtual bool reserveModule(int modelIndex);
    virtual bool releaseModule(int moduleIndex);
    virtual bool relinquishModule(int moduleIndex);                     // 注销板卡


    virtual bool reservePort(int moduleIndex, int portIndex);
    virtual bool releasePort(int moduleIndex,int portIndex);
    virtual bool relinquishPort(int moduleIndex,int portIndex);
    virtual void setPortTraffic(int moduleIndex,int portIndex,bool & tx);
    virtual bool portMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen);
    virtual void setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen);
    virtual bool portTimeLimit(int moduleIndex, int portIndex, int &limitMs);
    virtual void setPortTimeLimit(int moduleIndex, int portIndex, int limitMS);
    virtual void setPortRealSpeed(int moduleIndex, int portIndex, int speed_mbps);
    virtual bool portTxMode(int moduleIndex, int portIndex, QString &mode);
    virtual void setPortTxModeNormal(int moduleIndex, int portIndex);
    virtual void setPortTxModeStrict(int moduleIndex, int portIndex);
    virtual void setPortTxModeSequential(int moduleIndex, int portIndex);
    virtual bool portRateFraction(int moduleIndex, int portIndex, float &fraction);
    virtual void setPortRateFraction(int moduleIndex, int portIndex, float fraction);
    virtual bool portPacketRate(int moduleIndex, int portIndex, int &packetRate);
    virtual void setPortPacketRate(int moduleIndex, int portIndex, int packetRate);
    virtual bool portBitRate(int moduleIndex, int portIndex, double &bitRate);
    virtual void setPortBitRate(int moduleIndex, int portIndex, double bitRate);

    virtual bool portTxStatistics(int moduleIndex, int portIndex, int & bps,int & pps);


    virtual bool streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num);

    virtual void setStreamEnabled(int moduleIndex, int portIndex, int streamIndex, bool enabled = true);
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, float percent);
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, int packets);
    virtual bool setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index);
    virtual void setStreamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int limitNum);

    virtual void setStreamModifierInc(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    virtual void setStreamModifierDec(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    virtual void setStreamModifierRam(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    virtual void setStreamModifierRange(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);

    virtual bool addStream(int moduleIndex, int portIndex, CStream *stream);
    virtual bool delStream(int moduleIndex, int portIndex, CStream *stream);

    virtual bool acquireChassisInfo(CChassis *chassis);
    virtual bool acquireOneModuleInfo(int moduleIndex, CModule *module);
    virtual bool acquireAllModuleInfo(CChassis *chassis);
    virtual bool acquireOnePortInfo(int moduleIndex, int portIndex, CPort *port);
    virtual bool acquireAllPortInfo(CChassis *chassis);
    virtual bool acquireAllPortConfig(CChassis *chassis);
    virtual bool acquireAllPortFullConfig(CChassis *chassis);
    virtual bool acquireStreamInfo(int moduleIndex, int portIndex, int streamIndex, CStream *stream);


    virtual bool updateStream(int moduleIndex, int portIndex, CStream *stream);
    virtual bool updatePort(int moduleIndex, int portIndex, CPort* port);
    virtual bool updateModule(int moduleIndex, CModule* module);

    virtual bool updateStreamHeader(int moduleIndex, int portIndex, int streamIndex, CStream *stream);

    virtual bool startMonitorChassisStatus(CChassis* chassis);
    virtual bool stopMonitorChassisStatus();

    /************************************************************************/
    /* 辅助函数                                                             */
    /************************************************************************/
private:
    bool _requestChassisInfo();
    bool _fillChassisInfo(const QString& cmdText, CChassis *chassis);
    bool _requestOneModuleInfo(int moduleIndex);
    bool _fillModuleInfo(const QString& cmdText, CChassis *chassis);
    bool _requestOnePortInfo(int moduleIndex, int portIndex);
    bool _requestOnePortConfig(int moduleIndex, int portIndex);
    bool _requestOnePortFullConfig(int moduleIndex,int portIndex);
    bool _fillPortInfo(const QString& cmdText, CChassis *chassis);
    bool _fillPortConfig(const QString & cmdText, CChassis *chassis);
    bool _fillPortFullConfig(const QString & cmdText, CChassis *chassis);
    bool _fillStreamInfo(const QString & cmdText, CChassis *chassis);
    bool _fillModifierInfo(const QString & cmdText, CChassis *chassis);

    void connectRecv();
    void disconnectRecv();

private slots:
    void displayError(QAbstractSocket::SocketError);
    void receiveMsg();
    void pollChassis();

private:    
    bool sendData(E_ORDER type,S_ORDER & order = S_ORDER());
    /*
     *  可以同时下发多条指令
     *  @Note:
     *      多条指令的order必须要一样，不能冲突
     */
    bool sendData(E_ORDERS types, S_ORDER &order = S_ORDER());

    /*
     *  等待接收到数据
     */
    bool waitForRead(int mesc = -1);
    bool receiveData(QString &text = QString());

    bool queryPortCounts();

    bool _streamInit(S_ORDER &order, CStream *stream);
    bool _streamConfig(S_ORDER &order, CStream *stream);

private:
    QTcpSocket *m_pTcpSocket;
    ScriptCommand m_cmdMgr;

    bool m_bLogin;
    CXenaParser* m_parser;

    bool m_bQueryPortCount;
    int m_moduleCount;
    QList<int> m_portsCount;

    QMutex m_mutex;

    bool m_bSocketExceptions;

    bool m_bSuspendThread;
    bool m_bExitThread;
    CChassis* m_monitorChassis;
    QTimer *m_pollTimer;
    int m_pollCnt;
};


