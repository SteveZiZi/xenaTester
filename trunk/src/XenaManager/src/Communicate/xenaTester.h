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
    /* �������                                                             */
    /************************************************************************/
    const QString & chassisSerialNo();
    bool reserveChassis();         //ռ�û���
    bool releaseChassis();         //�ͷŻ���
    bool relinquishChassis();      //ע������
    bool chassisReservedOwner(QString &owner);

    /************************************************************************/
    /* �忨����                                                             */
    /************************************************************************/
    bool reserveModule(int moduleIndex);
    bool releaseModule(int moduleIndex);
    bool relinquishModule(int moduleIndex);
    bool moduleReservedOwner(int moduleIndex, QString &owner);

    /************************************************************************/
    /* �˿ڲ���                                                             */
    /************************************************************************/
    bool reservePort(int moduleIndex, int portIndex);
    bool releasePort(int moduleIndex, int portIndex);
    bool relinquishPort(int moduleIndex,int portIndex);
    bool portReservedOwner(int moduleIndex, int portIndex, QString &owner);
    bool portName(int moduleIndex, int portIndex, QString &name, QString &desc = QString(""));
    bool portCommunicationStatus(int moduleIndex, int portIndex, bool &linked, bool &actived);
    bool addStream(int moduleIndex, int portIndex, const ImportData *data);
    bool addStream(int moduleIndex, int portIndex, int streamIndex, const ImportData *data);    // ���ڴ������ļ��е���
    bool addStream(int moduleIndex, int portIndex, CStream *stream);
    bool delStream(int moduleIndex, int portIndex, CStream *stream);

    // ��ȡ�˿����ͷ����
    bool getPortMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen);
    // ���ö˿����ͷ����
    void setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen);

    // ��ȡ�˿ڷ���ʱ��
    bool getPortTimeLimit(int moduleIndex, int portIndex, int &limitMs);
    // ���ö˿ڷ���ʱ��
    void setPortTimeLimit(int moduleIndex, int portIndex, int limitMS);

    //  �˿��Ƿ����ڷ�������
    bool portIsTrafficing(int moduleIndex, int portIndex);
    bool portIsTrafficing(CPort* port);
    //  �˿ڰ����趨����ʼ���ʷ���
    bool portStartSpeedTraffic(int moduleIndex, int portIndex);
    bool portStartSpeedTraffic(CPort* port);
    //  �˿ڰ����趨����һ�����ʷ���
    bool portNextSpeedTraffic(int moduleIndex, int portIndex);
    bool portNextSpeedTraffic(CPort* port);
    //  �˿�ֹͣ����
    bool portStopTranffic(int moduleIndex, int portIndex);
    bool portStopTranffic(CPort* port);
    //  �˿���ɲ��Է���
    bool portFinishedStrategy(int moduleIndex, int portIndex);
    bool portFinishedStrategy(CPort* port);

    // �˿ڷ����������
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
    /* ������                                                               */
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
    /* ���õ��뵼���ӿ�                                                     */
    /************************************************************************/
    bool exportPort(CPort* port, const QString &xmlFileName);
    bool exportPort(int moduleIndex, int portIndex, const QString &xmlFileName);
    bool exportModule(CModule* module, const QString &xmlFileName);
    bool exportModule(int moduleIndex, const QString &xmlFileName);

    /*
     *  @Param In:
     *      xmlFileName:    xml�ļ���
     *  @Param In:
     *      moduleIndex:    �忨���
     *  @Param In:
     *      portIndex:      �˿����
     *  @Param Out:
     *      port:           �����������Ķ˿���
     */
    bool importPort(const QString &xmlFileName, int moduleIndex, int portIndex, CPort *port);
    /*
     *  @Param In:
     *      xmlFileName:    xml�ļ���
     *  @Param In:
     *      moduleIndex:    �忨���
     *  @Param Out:
     *      module:         �����������İ忨��
     */
    bool importModule(const QString &xmlFileName, int moduleIndex, CModule *module);

private:
    CChassis       m_chassis;
    IDevice        *m_commDev;

    bool           m_bLogin;
};