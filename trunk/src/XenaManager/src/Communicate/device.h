#pragma once

#include <QObject>
#include <QString>

#include "XenaManager/src/ScriptCommand/scriptcommand.h"

class CXenaParser;
class CChassis;
class CModule;
class CPort;
class CStream;
class IDevice
{
public:
    IDevice() { }
    virtual ~IDevice() {}

    virtual bool login(const QString &ip, int port, const QString &userName, const QString &pwd) = 0;
    virtual bool logout() = 0;
    virtual bool isLogin() = 0;

    /*
     *  保持机箱一直处于在线状态
     */
    virtual bool keepAlive() = 0;

    virtual bool reserveChassis() = 0;
    virtual bool releaseChassis() = 0;
    virtual bool relinquishChassis() = 0;                               // 注销机箱


    virtual bool reserveModule(int modelIndex) = 0;
    virtual bool releaseModule(int moduleIndex) = 0;
    virtual bool relinquishModule(int moduleIndex) = 0;                 // 注销板卡


    virtual bool reservePort(int modelIndex, int portIndex) = 0;
    virtual bool releasePort(int moduleIndex,int portIndex) = 0;
    virtual bool relinquishPort(int moduleIndex,int portIndex) = 0;
    virtual void setPortTraffic(int moduleIndex,int portIndex,bool & tx) = 0;
    virtual bool portMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen) = 0;
    virtual void setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen) = 0;
    virtual bool portTimeLimit(int moduleIndex, int portIndex, int &limitMs) = 0;
    virtual void setPortTimeLimit(int moduleIndex, int portIndex, int limitMS) = 0;
    virtual void setPortRealSpeed(int moduleIndex, int portIndex, int speed_mbps) = 0;
    virtual bool portTxMode(int moduleIndex, int portIndex, QString &mode) = 0;
    virtual void setPortTxModeNormal(int moduleIndex, int portIndex) = 0;
    virtual void setPortTxModeStrict(int moduleIndex, int portIndex) = 0;
    virtual void setPortTxModeSequential(int moduleIndex, int portIndex) = 0;
    virtual bool portRateFraction(int moduleIndex, int portIndex, float &fraction) = 0;
    virtual void setPortRateFraction(int moduleIndex, int portIndex, float fraction) = 0;
    virtual bool portPacketRate(int moduleIndex, int portIndex, int &packetRate) = 0;
    virtual void setPortPacketRate(int moduleIndex, int portIndex, int packetRate) = 0;
    virtual bool portBitRate(int moduleIndex, int portIndex, double &bitRate) = 0;
    virtual void setPortBitRate(int moduleIndex, int portIndex, double bitRate) = 0;

    virtual bool portTxStatistics(int moduleIndex, int portIndex, int & bps,int & pps) = 0;


    virtual bool streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num) = 0;

    virtual void setStreamEnabled(int moduleIndex, int portIndex, int streamIndex, bool enabled = true) = 0;
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, float percent) = 0;
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, int packets) = 0;
    virtual bool setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index) = 0;
    virtual void setStreamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int limitNum) = 0;  // 设置流条数限制

    virtual void setStreamModifierInc(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;
    virtual void setStreamModifierDec(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;
    virtual void setStreamModifierRam(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;
    virtual void setStreamModifierRange(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;

    virtual bool addStream(int moduleIndex, int portIndex, CStream *stream) = 0;
    virtual bool delStream(int moduleIndex, int portIndex, CStream *stream) = 0;

    virtual bool acquireChassisInfo(CChassis *chassis) = 0;
    virtual bool acquireOneModuleInfo(int moduleIndex, CModule *module) = 0;
    virtual bool acquireAllModuleInfo(CChassis *chassis) = 0;
    virtual bool acquireOnePortInfo(int moduleIndex, int portIndex, CPort *port) = 0;
    virtual bool acquireAllPortInfo(CChassis *chassis) = 0;
    virtual bool acquireAllPortConfig(CChassis *chassis) = 0;
    virtual bool acquireAllPortFullConfig(CChassis *chassis) = 0;

    virtual bool updateStream(int moduleIndex, int portIndex, CStream *stream) = 0;
    virtual bool updatePort(int moduleIndex, int portIndex, CPort* port) = 0;
    virtual bool updateModule(int moduleIndex, CModule* module) = 0;

    virtual bool updateStreamHeader(int moduleIndex, int portIndex, int streamIndex, CStream *stream) = 0;

    virtual bool startMonitorChassisStatus(CChassis* chassis) = 0;
    virtual bool stopMonitorChassisStatus() = 0;
};

/*
 *  获取通信设备
 *  @Note:
 *      返回的是一个静态全局指针，调用者不需要释放设备
 */
IDevice* GetCommDevice();


