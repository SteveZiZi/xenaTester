#pragma once

#include "chassis.h"
#include "module.h"
#include "port.h"
#include "stream.h"

class IDevice;
struct ImportData;

class CXenaTester
{
public:
    CXenaTester();
    ~CXenaTester();

    CChassis* chassis() {return &m_chassis;}
    IDevice* device() {return m_commDev;}

    bool login(const QString &ip, int port, const QString &userName, const QString &pwd);
    void logout();
    bool isLogin() const {return m_bLogin;}

    bool queryChassisInfo();

    /************************************************************************/
    /* 机箱操作                                                             */
    /************************************************************************/
    const QString & chassisSerialNo();
    bool reservedChassis();         //占用机箱

    /************************************************************************/
    /* 板卡操作                                                             */
    /************************************************************************/
    bool reservedModule(int moduleIndex);

    /************************************************************************/
    /* 端口操作                                                             */
    /************************************************************************/
    bool reservedPort(int moduleIndex, int portIndex);
    bool addStream(int moduleIndex, int portIndex, const ImportData *data);
    bool addStream(int moduleIndex, int portIndex, CStream *stream);

    //  端口是否正在发送数据
    bool portIsTrafficing(int moduleIndex, int portIndex);
    bool portIsTrafficing(CPort* port);
    //  端口按照设定的起始速率发送
    bool portStartSpeedTraffic(int moduleIndex, int portIndex);
    bool portStartSpeedTraffic(CPort* port);
    //  端口按照设定的下一个速率发送
    bool portNextSpeedTraffic(int moduleIndex, int portIndex);
    bool portNextSpeedTraffic(CPort* port);
    //  端口停止发送
    bool portStopTranffic(int moduleIndex, int portIndex);
    bool portStopTranffic(CPort* port);
    //  端口完成策略发送
    bool portFinishedStrategy(int moduleIndex, int portIndex);
    bool portFinishedStrategy(CPort* port);



private:
    CChassis       m_chassis;
    IDevice        *m_commDev;

    bool            m_bLogin;
};