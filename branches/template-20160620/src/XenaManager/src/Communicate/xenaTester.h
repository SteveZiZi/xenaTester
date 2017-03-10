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
    /* �������                                                             */
    /************************************************************************/
    const QString & chassisSerialNo();
    bool reservedChassis();         //ռ�û���

    /************************************************************************/
    /* �忨����                                                             */
    /************************************************************************/
    bool reservedModule(int moduleIndex);

    /************************************************************************/
    /* �˿ڲ���                                                             */
    /************************************************************************/
    bool reservedPort(int moduleIndex, int portIndex);
    bool addStream(int moduleIndex, int portIndex, const ImportData *data);
    bool addStream(int moduleIndex, int portIndex, CStream *stream);

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



private:
    CChassis       m_chassis;
    IDevice        *m_commDev;

    bool            m_bLogin;
};