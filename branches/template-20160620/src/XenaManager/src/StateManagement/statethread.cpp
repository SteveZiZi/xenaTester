#include "statethread.h"
#include "XenaManager/src/Communicate/communicate.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include <QTimer>
#include <QMutexLocker>

StateThread::StateThread(Communicate *communicate,QObject *parent)
	: QThread(parent)
{
	m_pUpdateTimer = new QTimer(this);
	connect(m_pUpdateTimer,SIGNAL(timeout()),this,SLOT(run()));
}

StateThread::~StateThread()
{
	QMutexLocker locker(&m_mutex);
}

void StateThread::start()
{
	m_pUpdateTimer->start(XenaManageSettings::stateUpdateTime());
}

void StateThread::run()
{
	QMutexLocker locker(&m_mutex);
	S_ORDER order;
	//m_pCommunicate->sendData(C_RESERVEDBY,order);

	for (int moduleCount = 0;moduleCount < m_moduleCount;moduleCount++)
	{
		S_ORDER moduleOrder;
		moduleOrder.moduleIndex = moduleCount;
		//m_pCommunicate->sendData(M_RESERVEDBY,moduleOrder);				// 查询板卡占用状态

		for (int portCount = 0;portCount < m_portCount[moduleCount];portCount++)
		{
			S_ORDER portOrder;
			portOrder.moduleIndex = moduleCount;
			portOrder.portIndex = portCount;
// 			m_pCommunicate->sendData(P_INTERFACE,portOrder);			// 查询端口的名称
// 			m_pCommunicate->sendData(P_RESERVEDBY,portOrder);			// 查询端口占用状态
// 			m_pCommunicate->sendData(P_TRAFFIC_QUERY,portOrder);		// 查询端口是否正在发送数据
// 			m_pCommunicate->sendData(P_RECEIVESYNC,portOrder);			// 查询端口的同步状态
		}
	}
}

void StateThread::getModuleCount(int moduleCount)
{
	m_moduleCount = moduleCount;
}

void StateThread::getPortCount(int moduleIndex,int portNum)
{
	m_portCount[moduleIndex] = portNum;
}
