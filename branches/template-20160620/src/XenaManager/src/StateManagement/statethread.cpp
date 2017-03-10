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
		//m_pCommunicate->sendData(M_RESERVEDBY,moduleOrder);				// ��ѯ�忨ռ��״̬

		for (int portCount = 0;portCount < m_portCount[moduleCount];portCount++)
		{
			S_ORDER portOrder;
			portOrder.moduleIndex = moduleCount;
			portOrder.portIndex = portCount;
// 			m_pCommunicate->sendData(P_INTERFACE,portOrder);			// ��ѯ�˿ڵ�����
// 			m_pCommunicate->sendData(P_RESERVEDBY,portOrder);			// ��ѯ�˿�ռ��״̬
// 			m_pCommunicate->sendData(P_TRAFFIC_QUERY,portOrder);		// ��ѯ�˿��Ƿ����ڷ�������
// 			m_pCommunicate->sendData(P_RECEIVESYNC,portOrder);			// ��ѯ�˿ڵ�ͬ��״̬
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
