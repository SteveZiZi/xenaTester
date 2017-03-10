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
	void loginResult(const QString & result);							// 登陆结果
	void chassisSerialNo(const QString & serialNo);						// 机箱序列号
	void chassisName(QString & name);									// 机箱名字
	void chassisReservedBy(QString & name);								// 获取机箱的占用者
	void portName(int moduleIndex,int portIndex,QString & name);		// 端口名字
	void portDiscription(int moduleIndex,int portIndex,QString & name); // 端口描述
	void moduleNameAndCount(int moduleIndex,QString & number);			// 板卡名字及数量
	void portCount(int moduleCount,const QStringList & number);			// 端口数量
	void chassisReservation(const QString & reservation);				// 机箱占用状态
	void moduleReservation(int moduleIndex,const QString & reservation);// 板卡占用状态
	void moduleReservedBy(int moduleIndex,QString & name);				// 获取板卡的占用者
	void portReservation(int moduleIndex,int portIndex,const QString & reservation);				// 端口占用状态
	void portReservedBy(int moduleIndex,int portIndex,QString & name);								// 获取端口的占用者
	void portReceiveSync(int moduleIndex,int portIndex,const QString & syncStatus);					// 端口同步状态
	void portTrafficStatus(int moduleIndex,int portIndex,const QString & trafficStatus);			// 端口传输数据状态
	void portTPLDMode(int moduleIndex,int portIndex,const QString & mode);							// 端口TPLD模式
	void portCheckSum(int moduleIndex,int portIndex,const QString & checkSum);						// 端口校验和
	void portMaxHeaderLength(int moduleIndex,int portIndex,const QString & length);					// 端口最大头部长度
	void portTimeLimit(int moduleIndex,int portIndex,const QString & microseconds);					// 获取端口时间限制
	void portSpeed(int moduleIndex,int portIndex,const QString & Mbps);								// 获取端口的实际速率
	void streamComment(int moduleIndex,int portIndex,int streamIndex,const QString & comment);		// 流描述
	void streamAbleStatus(int moduleIndex,int portIndex,int streamIndex,const QString & status);	// 获取流的使能状态
	void streamModifierCount(int moduleIndex,int portIndex,int streamIndex,const QString & number); // 流Modifier个数
	void streamPacketLimit(int moduleIndex,int portIndex,int streamIndex,const QString & number);	// 获取流的帧数限制
	void streamRatePrecent(int moduleIndex,int portIndex,int streamIndex,const QString & precent);	// 获取流的速率(百分比)
	void streamRatePacketsPerSec(int moduleIndex,int portIndex,int streamIndex,const QString & packets);//获取流的速率(帧每秒)

public slots:
	bool createConnect(const QString & ip,const int & port,const QString & pwd);					// 建立连接
	bool receiveData();													// 获取接收的数据

private slots:															
	void keepAlive();													// 保持在线
	void displayError(QAbstractSocket::SocketError);

private:
	void parseReceiveData(const QString & receiveData);					// 解析socket接收到的数据
	void dealAbnormalReply(const QString & receiveData);				// 处理异常的回复信息
	void dealSingleReply(const QStringList & singleList);				// 处理单个回复信息
	void dealChassisReply(const QStringList & chassisInfoList);			// 处理机箱的回复信息
	void dealModuleAndPortReply(const QStringList & infoList);			// 处理板卡和端口的回复信息
	void dealModuleReply(const QStringList & moduleList);				// 处理板卡的回复信息
	void dealPortReply(const QStringList & portInfoList);				// 处理端口的回复信息
	void dealStreamReply(const QStringList & streamInfoList);			// 处理流的回复信息
	void dealOtherReply(const QStringList & replyList);					// 处理其他回复信息

private:
    QTcpSocket *m_pTcpSocket;
	QTimer *m_pKeepAliveTimer;
	unsigned m_orderCount;
	bool m_connected;													// 登陆标志
};

#endif // COMMUNICATE_H
