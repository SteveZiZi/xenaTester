#pragma once

#include <QObject>
#include <QString>

#include "XenaManager/src/ScriptCommand/scriptcommand.h"

class CXenaParser;
class CChassis;
class CModule;
class CPort;
class CStream;
class IDevice : public QObject
{
    Q_OBJECT
public:
    IDevice(QObject* parent = 0) : QObject(parent) {}
    virtual ~IDevice() {}

    virtual bool login(const QString &ip, int port, const QString &userName, const QString &pwd) = 0;
    virtual bool logout() = 0;
    virtual bool isLogin() = 0;

    /*
     *  保持机箱一直处于在线状态
     */
    virtual bool keepAlive() = 0;
    /*
     *  机箱名字
     */
    virtual bool chassisName(QString &name, QString &desc = QString("")) = 0;
    //virtual bool chassisName() = 0;
    /*
     *  机箱序列号
     */
    virtual bool chassisSerialNo(QString &serialNo) = 0;
    /*
     *  机箱使用者
     *  
     *  @Return:
     *      true：成功获取机箱使用者状态，如果未有使用者占用机箱，owner = ""
     *      false:通讯异常
     */
    virtual bool chassisReservedOwner(QString &owner) = 0;
    virtual bool reserveChassis() = 0;
    virtual bool releaseChassis() = 0;
    virtual bool relinquishChassis() = 0;                               // 注销机箱

    virtual int  moduleCount() = 0;
    virtual bool moduleName(int modelIndex, QString &name, QString &desc = QString("")) = 0;
    /*
     *  板卡使用者
     *  
     *  @Return:
     *      true：成功获取机箱使用者状态，如果未有使用者占用板卡，owner = ""
     *      false:通讯异常
     */
    virtual bool moduleReservedOwner(int modelIndex, QString &owner) = 0;
    virtual bool reserveModule(int modelIndex) = 0;
    virtual bool releaseModule(int moduleIndex) = 0;
    virtual bool relinquishModule(int moduleIndex) = 0;                 // 注销板卡


    virtual int  portCount(int moduleIndex) = 0;
    virtual bool portName(int moduleIndex, int portIndex, QString &name, QString &desc = QString("")) = 0;
    /*
     *  端口使用者
     *  
     *  @Return:
     *      true：成功获取机箱使用者状态，如果未有使用者占用板卡，owner = ""
     *      false:通讯异常
     */
    virtual bool portReservedOwner(int modelIndex, int portIndex, QString &owner) = 0;
    virtual bool reservePort(int modelIndex, int portIndex) = 0;
    virtual bool releasePort(int moduleIndex,int portIndex) = 0;
    virtual bool relinquishPort(int moduleIndex,int portIndex) = 0;
    virtual bool portCommunicationStatus(int modelIndex, int portIndex, bool &linked, bool &actived) = 0;
    virtual bool portTPLDModel(int moduleIndex, int portIndex, QString &model) = 0;
    virtual bool portCheckSum(int moduleIndex, int portIndex, bool &enable) = 0;
    virtual bool portMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen) = 0;
    virtual bool portTimeLimit(int moduleIndex, int portIndex, int &limitMS) = 0;
    virtual bool portRealSpeed(int moduleIndex, int portIndex, int &speed_mbps) = 0;

    virtual void setPortTraffic(int moduleIndex,int portIndex,bool & tx) = 0;
    virtual void setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen) = 0;
    virtual void setPortTimeLimit(int moduleIndex, int portIndex, int limitMS) = 0;
    virtual void setPortRealSpeed(int moduleIndex, int portIndex, int speed_mbps) = 0;


    virtual bool portTxStatistics(int moduleIndex, int portIndex, int & bps,int & pps) = 0;


    virtual bool streamCount(int moduleIndex, int portIndex, int &streamIndex) = 0;
    virtual bool streamComment(int moduleIndex, int portIndex, int streamIndex, QString & comment) = 0;
    virtual bool streamEnabled(int moduleIndex, int portIndex, int streamIndex, bool &enabled) = 0;
    virtual bool streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num) = 0;
    virtual bool streamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int &limitNum) = 0;
    virtual bool streamRate(int moduleIndex, int portIndex, int streamIndex, float &percent) = 0;   // 获取流的速率(百分比)
    virtual bool streamRate(int moduleIndex, int portIndex, int streamIndex, int &packets) = 0;     //获取流的速率(帧每秒)
    
    virtual void setStreamEnabled(int moduleIndex, int portIndex, int streamIndex, bool enabled = true) = 0;
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, float percent) = 0;
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, int packets) = 0;
    virtual void setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index) = 0;
    virtual void setStreamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int limitNum) = 0;  // 设置流条数限制

    virtual void setStreamModifierInc(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;
    virtual void setStreamModifierDec(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;
    virtual void setStreamModifierRam(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;
    virtual void setStreamModifierRange(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify) = 0;

    virtual bool addStream(int moduleIndex, int portIndex, CStream *stream) = 0;
    virtual bool delStream(int moduleIndex, int portIndex, CStream *stream) = 0;

    virtual bool acquireChassisInfo(CChassis *chassis) = 0;
    virtual bool acquireModuleInfo(int moduleIndex, CModule *module) = 0;
    virtual bool acquirePortInfo(int moduleIndex, int portIndex, CPort *port) = 0;

};

/*
 *  获取通信设备
 *  @Note:
 *      返回的是一个静态全局指针，调用者不需要释放设备
 */
IDevice* GetCommDevice();


