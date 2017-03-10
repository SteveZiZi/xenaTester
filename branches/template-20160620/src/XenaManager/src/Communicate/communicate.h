#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#include "XenaManager/src/ScriptCommand/scriptcommand.h"
#include <QObject>
#include <QTcpSocket>

#define LOGINSUCCESSFUL "successful"
#define LOGINFALSE "false"

enum{
	SIGNALREPLY = 1,
	CHASSISREPLY,
	MODULANDPORTEREPLY,
	STREAMREPLY
};

class QTimer;
class Communicate : public QObject
{
    Q_OBJECT
public:
    explicit Communicate(QObject *parent = 0);
    bool sendData(E_ORDER type,S_ORDER & order);
    bool closeConnect();
	QString dataFromSocket;

signals:
	void loginResult(const QString & result);							// ��½���
	void chassisSerialNo(const QString & serialNo);						// �������к�
	void chassisName(QString & name);									// ��������
	void chassisReservedBy(QString & name);								// ��ȡ�����ռ����
	void portName(int moduleIndex,int portIndex,QString & name);		// �˿�����
	void portDiscription(int moduleIndex,int portIndex,QString & name); // �˿�����
	void moduleNameAndCount(int moduleIndex,QString & number);			// �忨���ּ�����
	void portCount(int moduleCount,const QStringList & number);			// �˿�����
	void chassisReservation(const QString & reservation);				// ����ռ��״̬
	void moduleReservation(int moduleIndex,const QString & reservation);// �忨ռ��״̬
	void moduleReservedBy(int moduleIndex,QString & name);				// ��ȡ�忨��ռ����
	void portReservation(int moduleIndex,int portIndex,const QString & reservation);				// �˿�ռ��״̬
	void portReservedBy(int moduleIndex,int portIndex,QString & name);								// ��ȡ�˿ڵ�ռ����
	void portReceiveSync(int moduleIndex,int portIndex,const QString & syncStatus);					// �˿�ͬ��״̬
	void portTrafficStatus(int moduleIndex,int portIndex,const QString & trafficStatus);			// �˿ڴ�������״̬
	void portTPLDMode(int moduleIndex,int portIndex,const QString & mode);							// �˿�TPLDģʽ
	void portCheckSum(int moduleIndex,int portIndex,const QString & checkSum);						// �˿�У���
	void portMaxHeaderLength(int moduleIndex,int portIndex,const QString & length);					// �˿����ͷ������
	void portTimeLimit(int moduleIndex,int portIndex,const QString & microseconds);					// ��ȡ�˿�ʱ������
	void portSpeed(int moduleIndex,int portIndex,const QString & Mbps);								// ��ȡ�˿ڵ�ʵ������
	void streamComment(int moduleIndex,int portIndex,int streamIndex,const QString & comment);		// ������
	void streamAbleStatus(int moduleIndex,int portIndex,int streamIndex,const QString & status);	// ��ȡ����ʹ��״̬
	void streamModifierCount(int moduleIndex,int portIndex,int streamIndex,const QString & number); // ��Modifier����
	void streamPacketLimit(int moduleIndex,int portIndex,int streamIndex,const QString & number);	// ��ȡ����֡������
	void streamRatePrecent(int moduleIndex,int portIndex,int streamIndex,const QString & precent);	// ��ȡ��������(�ٷֱ�)
	void streamRatePacketsPerSec(int moduleIndex,int portIndex,int streamIndex,const QString & packets);//��ȡ��������(֡ÿ��)

public slots:
	bool createConnect(const QString & ip,const int & port,const QString & pwd);					// ��������
	bool receiveData();													// ��ȡ���յ�����

private slots:															
	void keepAlive();													// ��������
	void displayError(QAbstractSocket::SocketError);

private:
	void parseReceiveData(const QString & receiveData);					// ����socket���յ�������
	void dealAbnormalReply(const QString & receiveData);				// �����쳣�Ļظ���Ϣ
	void dealSingleReply(const QStringList & singleList);				// �������ظ���Ϣ
	void dealChassisReply(const QStringList & chassisInfoList);			// �������Ļظ���Ϣ
	void dealModuleAndPortReply(const QStringList & infoList);			// ����忨�Ͷ˿ڵĻظ���Ϣ
	void dealModuleReply(const QStringList & moduleList);				// ����忨�Ļظ���Ϣ
	void dealPortReply(const QStringList & portInfoList);				// ����˿ڵĻظ���Ϣ
	void dealStreamReply(const QStringList & streamInfoList);			// �������Ļظ���Ϣ
	void dealOtherReply(const QStringList & replyList);					// ���������ظ���Ϣ

private:
    QTcpSocket *m_pTcpSocket;
	QTimer *m_pKeepAliveTimer;
	unsigned m_orderCount;
	bool m_connected;													// ��½��־
};

#endif // COMMUNICATE_H
