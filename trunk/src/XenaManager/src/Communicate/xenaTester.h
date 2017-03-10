#pragma once

#include "chassis.h"
#include "module.h"
#include "port.h"
#include "stream.h"
#include "XenaManager/src/ScriptCommand/scriptcommand.h"

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

    bool keepAlive();

    bool queryChassisInfo();

    /************************************************************************/
    /* 机箱操作                                                             */
    /************************************************************************/
    const QString & chassisSerialNo();
    bool reserveChassis();         //占用机箱
    bool releaseChassis();         //释放机箱
    bool relinquishChassis();      //注销机箱
    bool chassisReservedOwner(QString &owner);

    /************************************************************************/
    /* 板卡操作                                                             */
    /************************************************************************/
    bool reserveModule(int moduleIndex);
    bool releaseModule(int moduleIndex);
    bool relinquishModule(int moduleIndex);
    bool moduleReservedOwner(int moduleIndex, QString &owner);

    /************************************************************************/
    /* 端口操作                                                             */
    /************************************************************************/
    bool reservePort(int moduleIndex, int portIndex);
    bool releasePort(int moduleIndex, int portIndex);
    bool relinquishPort(int moduleIndex,int portIndex);
    bool portReservedOwner(int moduleIndex, int portIndex, QString &owner);
    bool portName(int moduleIndex, int portIndex, QString &name, QString &desc = QString(""));
    bool portCommunicationStatus(int moduleIndex, int portIndex, bool &linked, bool &actived);
    bool addStream(int moduleIndex, int portIndex, const ImportData *data);
    bool addStream(int moduleIndex, int portIndex, int streamIndex, const ImportData *data);    // 用于从配置文件中导入
    bool addStream(int moduleIndex, int portIndex, CStream *stream);
    bool delStream(int moduleIndex, int portIndex, CStream *stream);

    // 获取端口最大头长度
    bool getPortMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen);
    // 设置端口最大头长度
    void setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen);

    // 获取端口发送时间
    bool getPortTimeLimit(int moduleIndex, int portIndex, int &limitMs);
    // 设置端口发送时间
    void setPortTimeLimit(int moduleIndex, int portIndex, int limitMS);

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

    // 端口发送速率相关
    bool portTxMode(int moduleIndex, int portIndex, QString &mode);
    void setPortTxModeNormal(int moduleIndex, int portIndex);
    void setPortTxModeStrict(int moduleIndex, int portIndex);
    void setPortTxModeSequential(int moduleIndex, int portIndex);
    bool portRateFraction(int moduleIndex, int portIndex, float &fraction);
    void setPortRateFraction(int moduleIndex, int portIndex, float fraction);
    bool portPacketRate(int moduleIndex, int portIndex, int &packetRate);
    void setPortPacketRate(int moduleIndex, int portIndex, int packetRate);
    bool portBitRate(int moduleIndex, int portIndex, double &bitRate);
    void setPortBitRate(int moduleIndex, int portIndex, double bitRate);

    /************************************************************************/
    /* 流操作                                                               */
    /************************************************************************/
    bool updateStreamHeader(int moduleIndex, int portIndex, int streamIndex, CStream *stream);

    bool streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num);
    bool setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index);
    void setStreamModifierInc(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    void setStreamModifierDec(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    void setStreamModifierRam(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    void setStreamModifierRange(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    void setStreamEnabled(int moduleIndex, int portIndex, int streamIndex, bool enabled = true);

    /************************************************************************/
    /* 配置导入导出接口                                                     */
    /************************************************************************/
    bool exportPort(CPort* port, const QString &xmlFileName);
    bool exportPort(int moduleIndex, int portIndex, const QString &xmlFileName);
    bool exportModule(CModule* module, const QString &xmlFileName);
    bool exportModule(int moduleIndex, const QString &xmlFileName);

    /*
     *  @Param In:
     *      xmlFileName:    xml文件名
     *  @Param In:
     *      moduleIndex:    板卡序号
     *  @Param In:
     *      portIndex:      端口序号
     *  @Param Out:
     *      port:           由配置生产的端口类
     */
    bool importPort(const QString &xmlFileName, int moduleIndex, int portIndex, CPort *port);
    /*
     *  @Param In:
     *      xmlFileName:    xml文件名
     *  @Param In:
     *      moduleIndex:    板卡序号
     *  @Param Out:
     *      module:         由配置生产的板卡类
     */
    bool importModule(const QString &xmlFileName, int moduleIndex, CModule *module);

private:
    CChassis       m_chassis;
    IDevice        *m_commDev;

    bool           m_bLogin;
};