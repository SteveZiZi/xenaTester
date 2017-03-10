#include "communicate.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include <QTimer>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

#define LINEBREAK "\n"
#define SLASHMARK "/"

Communicate::Communicate(QObject *parent) :
    QObject(parent)
{
	m_connected = false;
    m_pTcpSocket = new QTcpSocket(this);
	m_pKeepAliveTimer = new QTimer(this);
	unsigned int keepAliveTime = XenaManageSettings::breathTime();
	m_pKeepAliveTimer->start(keepAliveTime);
	
	connect(m_pTcpSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
	connect(m_pKeepAliveTimer,SIGNAL(timeout()),this,SLOT(keepAlive()));
	connect(m_pTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
}

bool Communicate::createConnect(const QString & ip,const int & port,const QString & pwd)
{
	//tcpSocket->abort();
    m_pTcpSocket->connectToHost(ip,port);
	if(!m_pTcpSocket->waitForConnected(XenaManageSettings::waitConnectTime()))
    {
        qDebug() << "disConnected!";
		emit loginResult(LOGINFALSE);
		return false;
    }
	else
	{
		qDebug() << "connected";
		return true;	
	}
}

bool Communicate::sendData(E_ORDER type,S_ORDER & order)
{
	qDebug() << "*******send:";
	ScriptCommand command;
	QByteArray data;
	data = command.cmdToByteArray(type,order);
 	qDebug() << data;
	if (-1 == m_pTcpSocket->write(data,data.size()))
	{
		return false;
	}
	else
	{
		if (C_LOGON == type)
		{
			if (m_pTcpSocket->waitForReadyRead(XenaManageSettings::waitForReadReadyTime()))
			{
				if (dataFromSocket.contains("<OK>"))
				{
					emit loginResult(LOGINSUCCESSFUL);
					m_connected = true;
				}
			}
			else
			{
				emit loginResult(LOGINFALSE);
				m_connected = false;
			}
		}
		return true;
	}
}

bool Communicate::receiveData()
{
	dataFromSocket.clear();
	QByteArray backArr = m_pTcpSocket->readAll();
	if (backArr.isEmpty())
	{
		return false;
	}
	else
	{
		qDebug() << "=======Communicate::receiveData=========";

		dataFromSocket = QString(backArr);
		qDebug() << dataFromSocket;

		QStringList dataPackList = dataFromSocket.split(LINEBREAK);
		Q_FOREACH(QString pack, dataPackList) 
		{
			parseReceiveData(pack);
		}
		return true;
	}	
}

bool Communicate::closeConnect()
{
	m_pTcpSocket->disconnectFromHost();
	return true;
}

void Communicate::keepAlive()
{
	if (!m_connected)
	{
		return;
	}
	else
	{
		S_ORDER order;
		sendData(C_KEEPALIVE,order);
	}	
}

void Communicate::displayError(QAbstractSocket::SocketError socketError)
{
	switch(socketError)
	{
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::warning(0,tr("警告"),tr("网络链接不成功，请检查网络"),QString(tr("确定")));
		break;
	case QAbstractSocket::RemoteHostClosedError:
		QMessageBox::warning(0,tr("警告"),tr("网络链接断开，请检查网络"),QString(tr("确定")));
		qApp->exit(0);
		break;
	case QAbstractSocket::SocketTimeoutError:
		QMessageBox::warning(0,tr("警告"),tr("网络链接超时，请重新链接"),QString(tr("确定")));
		break;
	case QAbstractSocket::SocketAccessError:
		QMessageBox::warning(0,tr("警告"),tr("网络链接失败，请重新输入IP和端口号"),QString(tr("确定")));
		break;
	case QAbstractSocket::HostNotFoundError:
		QMessageBox::warning(0,tr("警告"),tr("网络链接失败，请重新输入IP和端口号"),QString(tr("确定")));
		break;
	default:
		break;
	}
}

void Communicate::parseReceiveData(const QString & receiveData)
{
	if(receiveData.contains("#Syntax error",Qt::CaseInsensitive))
	{
		dealAbnormalReply(receiveData);
		return;
	}

	QStringList splitList;
	splitList = receiveData.split("  ");
	int splitListSize = splitList.size();

	if (splitList[0].contains("C_PORTCOUNTS",Qt::CaseInsensitive))
	{
		QString portInPerModule = splitList[1];
		QStringList portInPerModuleList = portInPerModule.split(" ");
		int moduleCount = portInPerModuleList.size();
		QStringList portNum;
		for (int i = 0; i < moduleCount;i++)
		{
			portNum.append(portInPerModuleList[i]);
		}
		emit portCount(moduleCount,portNum);
	}

	switch(splitListSize)
	{
	case SIGNALREPLY:
		dealSingleReply(splitList);
		break;
	case CHASSISREPLY:
		dealChassisReply(splitList);
		break;
	case MODULANDPORTEREPLY:
		dealModuleAndPortReply(splitList);
		break;
	case STREAMREPLY:
		dealStreamReply(splitList);
		break;
	default:
		dealAbnormalReply(receiveData);
		break;
	}
}

void Communicate::dealAbnormalReply(const QString & receiveData)
{
	qDebug() << "******* abnormal reply";
}

void Communicate::dealSingleReply(const QStringList & singleList)
{
	if (singleList.isEmpty())
	{
		return;
	}

	QString tempStr = singleList[singleList.size() - 1];
	if (tempStr.contains("OK",Qt::CaseInsensitive))
	{
		
	}
	else if (tempStr.contains("NOTRESERVED",Qt::CaseInsensitive))
	{
		qDebug() << "****** Not Reserved";
	}
	else if (tempStr.contains("NOTREADABLE",Qt::CaseInsensitive))
	{
		qDebug() << "****** Not Readable";
	}
	else if (tempStr.contains("NOTWRITABLE",Qt::CaseInsensitive))
	{
		qDebug() << "******* Not Writable";
	}
	else if (tempStr.contains("NOTVALID",Qt::CaseInsensitive))
	{
		qDebug() << "******* Notvalid";
	}
	else if (tempStr.contains("BADMODULE",Qt::CaseInsensitive))
	{
		qDebug() << "******* Badmodule";
	}
	else if (tempStr.contains("BADINDEX",Qt::CaseInsensitive))
	{
		qDebug() << "******* Badindex";
	}
	else if (tempStr.contains("BADSIZE",Qt::CaseInsensitive))
	{
		qDebug() << "******* Badsize";
	}
	else if (tempStr.contains("BADVALUE",Qt::CaseInsensitive))
	{
		qDebug() << "******* Badvalue";
	}
	else if (tempStr.contains("FAILED",Qt::CaseInsensitive))
	{
		qDebug() << "******* Failed";
	}
}

void Communicate::dealChassisReply(const QStringList & chassisInfoList)
{
	if (chassisInfoList.size() != 2)
	{
		return;
	}

	QString commandStr = chassisInfoList[0];
	QString signalStr = chassisInfoList[1];
	if (!commandStr.contains("C_",Qt::CaseInsensitive))
	{
		return;
	}

	if (commandStr.contains("C_KEEPALIVE",Qt::CaseInsensitive))
	{

	}
	else if (commandStr.contains("C_NAME",Qt::CaseInsensitive))
	{
		emit chassisName(signalStr);
	}
	else if (commandStr.contains("C_SERIALNO",Qt::CaseInsensitive))
	{
		emit chassisSerialNo(signalStr);
	}
	else if (commandStr.contains("C_RESERVATION",Qt::CaseInsensitive))
	{
		emit chassisReservation(signalStr);
	}
	else if(commandStr.contains("C_RESERVEDBY",Qt::CaseInsensitive))
	{
		emit chassisReservedBy(signalStr);
	}
}

void Communicate::dealModuleAndPortReply(const QStringList & infoList)
{
	if (infoList.size() != 3)
	{
		return;
	}
	
	QString moduleAndPortInfo = infoList[1];
	if (moduleAndPortInfo.contains("M_",Qt::CaseInsensitive))
	{
		dealModuleReply(infoList);
	}
	else if (moduleAndPortInfo.contains("P_",Qt::CaseInsensitive))
	{
		dealPortReply(infoList);
	}
}

void Communicate::dealModuleReply(const QStringList & moduleList)
{
	QString commandStr = moduleList[1];
	int moduleIndex = moduleList[0].toInt();
	if (moduleIndex < 0)
	{
		return;
	}
	QString signalStr = moduleList[2];

	if (commandStr.contains("M_MODEL",Qt::CaseInsensitive))
	{
		emit moduleNameAndCount(moduleIndex,signalStr);
	}
	else if (commandStr.contains("M_RESERVATION",Qt::CaseInsensitive))
	{
		emit moduleReservation(moduleIndex,signalStr);
	}
	else if (commandStr.contains("M_RESERVEDBY",Qt::CaseInsensitive))
	{
		emit moduleReservedBy(moduleIndex,signalStr);
	}
}

void Communicate::dealPortReply(const QStringList & portInfoList)
{
	QString commandStr = portInfoList[1];
	QString moduleIndexAndPortIndex = portInfoList[0];
	if (moduleIndexAndPortIndex.isEmpty())
	{
		return;
	}
	if (!moduleIndexAndPortIndex.contains(SLASHMARK))
	{
		return;
	}
	QStringList moduleAndPortList = portInfoList[0].split(SLASHMARK);
	int moduleIndex = moduleAndPortList[0].toInt();
	int portIndex = moduleAndPortList[1].toInt();
	QString signalStr = portInfoList[2];

	if (commandStr.contains("P_INTERFACE",Qt::CaseInsensitive))
	{
		emit portName(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_COMMENT",Qt::CaseInsensitive))
	{
		emit portDiscription(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_RESERVATION",Qt::CaseInsensitive))
	{
		emit portReservation(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_RECEIVESYNC",Qt::CaseInsensitive))
	{
		emit portReceiveSync(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_TRAFFIC",Qt::CaseInsensitive))
	{
		emit portTrafficStatus(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_TPLDMODE",Qt::CaseInsensitive))
	{
		emit portTPLDMode(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_CHECKSUM",Qt::CaseInsensitive))
	{
		emit portCheckSum(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_MAXHEADERLENGTH",Qt::CaseInsensitive))
	{
		emit portMaxHeaderLength(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_RESERVEDBY",Qt::CaseInsensitive))
	{
		emit portReservedBy(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_TXTIMELIMIT",Qt::CaseInsensitive))
	{
		emit portTimeLimit(moduleIndex,portIndex,signalStr);
	}
	else if (commandStr.contains("P_SPEED",Qt::CaseInsensitive))
	{
		emit portSpeed(moduleIndex,portIndex,signalStr);
	}
}
inline int getNumberFormString(QString str)
{
	QString tempStr;
	for(int i = 0; i < str.length(); i++)
	{
		if(str[i].isDigit())
		{
			tempStr.append(str[i]);
		}	
	}
	return tempStr.toInt();
}

void Communicate::dealStreamReply(const QStringList & streamInfoList)
{
	if (streamInfoList.size() != 4)
	{
		return;
	}

	QString commandStr = streamInfoList[1];

	QString moduleIndexAndPortIndex = streamInfoList[0];
	if (moduleIndexAndPortIndex.isEmpty())
	{
		return;
	}
	if (!moduleIndexAndPortIndex.contains(SLASHMARK))
	{
		return;
	}

	QStringList moduleAndPortList = streamInfoList[0].split(SLASHMARK);
	int moduleIndex = moduleAndPortList[0].toInt();
	int portIndex = moduleAndPortList[1].toInt();
	int streamIndex = getNumberFormString(streamInfoList[2]);
	QString signalStr = streamInfoList[3];

	if (!commandStr.contains("PS_",Qt::CaseInsensitive))
	{
		return;
	}
	
	if (commandStr.contains("PS_COMMENT",Qt::CaseInsensitive))
	{
		emit streamComment(moduleIndex,portIndex,streamIndex,signalStr);
	}
	else if (commandStr.contains("PS_MODIFIERCOUNT",Qt::CaseInsensitive))
	{
		emit streamModifierCount(moduleIndex,portIndex,streamIndex,signalStr);
	}
	else if (commandStr.contains("PS_PACKETLIMIT",Qt::CaseInsensitive))
	{
		emit streamPacketLimit(moduleIndex,portIndex,streamIndex,signalStr);
	}
	else if (commandStr.contains("PS_RATEFRACTION",Qt::CaseInsensitive))
	{
		emit streamRatePrecent(moduleIndex,portIndex,streamIndex,signalStr);
	}
	else if (commandStr.contains("PS_RATEPPS",Qt::CaseInsensitive))
	{
		emit streamRatePacketsPerSec(moduleIndex,portIndex,streamIndex,signalStr);
	}
	else if (commandStr.contains("PS_ENABLE",Qt::CaseInsensitive))
	{
		emit streamAbleStatus(moduleIndex,portIndex,streamIndex,signalStr);
	}
}

void Communicate::dealOtherReply(const QStringList & replyList)
{

}