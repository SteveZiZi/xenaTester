#pragma once

#include "device.h"

#include <QtNetwork/QTcpSocket>
#include <QList>
#include <QMutex>

class CXenaParser;

class CDeviceImpl : public IDevice
{
    Q_OBJECT
public:
    CDeviceImpl(QObject* parent = 0) ;
    virtual ~CDeviceImpl();

    virtual bool login(const QString &ip, int port, const QString &userName, const QString &pwd);
    virtual bool logout();
    
    bool isLogin() {return m_bLogin;}

    /*
     *  ���ֻ���һֱ��������״̬
     */
    virtual bool keepAlive();
    /*
     *  ��������
     */
    virtual bool chassisName(QString &name, QString &desc = QString(""));
    /*
     *  �������к�
     */
    virtual bool chassisSerialNo(QString &serialNo);
    /*
     *  ����ʹ����
     *  
     *  @Return:
     *      true���ɹ���ȡ����ʹ����״̬�����δ��ʹ����ռ�û��䣬owner = ""
     *      false:ͨѶ�쳣
     */
    virtual bool chassisReservedOwner(QString &owner);
    virtual bool reserveChassis();
    virtual bool releaseChassis();
    virtual bool relinquishChassis();                                   // ע������

    virtual int  moduleCount();
    virtual bool moduleName(int modelIndex, QString &name, QString &desc = QString(""));
    virtual int  allModuleName(QStringList& nameList);
    /*
     *  �忨ʹ����
     *  
     *  @Return:
     *      true���ɹ���ȡ����ʹ����״̬�����δ��ʹ����ռ�ð忨��owner = ""
     *      false:ͨѶ�쳣
     */
    virtual bool moduleReservedOwner(int modelIndex, QString &owner);
    virtual bool reserveModule(int modelIndex);
    virtual bool releaseModule(int moduleIndex);
    virtual bool relinquishModule(int moduleIndex);                     // ע���忨


    virtual int  portCount(int moduleIndex);
    virtual bool portName(int moduleIndex, int portIndex, QString &name, QString &desc = QString(""));
    /*
     *  �˿�ʹ����
     *  
     *  @Return:
     *      true���ɹ���ȡ����ʹ����״̬�����δ��ʹ����ռ�ð忨��owner = ""
     *      false:ͨѶ�쳣
     */
    virtual bool portReservedOwner(int moduleIndex, int portIndex, QString &owner);
    virtual bool reservePort(int moduleIndex, int portIndex);
    virtual bool releasePort(int moduleIndex,int portIndex);
    virtual bool relinquishPort(int moduleIndex,int portIndex);
    virtual bool portCommunicationStatus(int moduleIndex, int portIndex, bool &linked, bool &actived);
    virtual bool portTPLDModel(int moduleIndex, int portIndex, QString &model);
    virtual bool portCheckSum(int moduleIndex, int portIndex, bool &enable);
    virtual bool portMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen);
    virtual bool portTimeLimit(int moduleIndex, int portIndex, int &limitMS);
    virtual bool portRealSpeed(int moduleIndex, int portIndex, int &speed_mbps);

    virtual void setPortTraffic(int moduleIndex,int portIndex,bool & tx);
    virtual void setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen);
    virtual void setPortTimeLimit(int moduleIndex, int portIndex, int limitMS);
    virtual void setPortRealSpeed(int moduleIndex, int portIndex, int speed_mbps);


    virtual bool portTxStatistics(int moduleIndex, int portIndex, int & bps,int & pps);


    virtual bool streamCount(int moduleIndex, int portIndex, int &streamIndex);
    virtual bool streamComment(int moduleIndex, int portIndex, int streamIndex, QString & comment);
    virtual bool streamEnabled(int moduleIndex, int portIndex, int streamIndex, bool &enabled);
    virtual bool streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num);
    virtual bool streamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int &limitNum);
    virtual bool streamRate(int moduleIndex, int portIndex, int streamIndex, float &percent);   // ��ȡ��������(�ٷֱ�)
    virtual bool streamRate(int moduleIndex, int portIndex, int streamIndex, int &packets);     //��ȡ��������(֡ÿ��)

    virtual void setStreamEnabled(int moduleIndex, int portIndex, int streamIndex, bool enabled = true);
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, float percent);
    virtual void setStreamRate(int moduleIndex, int portIndex, int streamIndex, int packets);
    virtual void setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index);
    virtual void setStreamPacketLimit(int moduleIndex, int portIndex, int streamIndex, int limitNum);

    virtual void setStreamModifierInc(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    virtual void setStreamModifierDec(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    virtual void setStreamModifierRam(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);
    virtual void setStreamModifierRange(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify);

    virtual bool addStream(int moduleIndex, int portIndex, CStream *stream);
    virtual bool delStream(int moduleIndex, int portIndex, CStream *stream);

    virtual bool acquireChassisInfo(CChassis *chassis);
    virtual bool acquireModuleInfo(int moduleIndex, CModule *module);
    virtual bool acquirePortInfo(int moduleIndex, int portIndex, CPort *port);
    virtual bool acquireStreamInfo(int moduleIndex, int portIndex, int streamIndex, CStream *stream);


    virtual bool updateStream(int moduleIndex, int portIndex, CStream *stream);
    virtual bool updatePort(int moduleIndex, int portIndex, CPort* port);
    virtual bool updateModule(int moduleIndex, CModule* module);

private slots:
    void displayError(QAbstractSocket::SocketError);

private:    
    bool sendData(E_ORDER type,S_ORDER & order = S_ORDER());
    /*
     *  ����ͬʱ�·�����ָ��
     *  @Note:
     *      ����ָ���order����Ҫһ�������ܳ�ͻ
     */
    bool sendData(E_ORDERS types, S_ORDER &order = S_ORDER());

    /*
     *  �ȴ����յ�����
     */
    bool waitForRead();
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
};

