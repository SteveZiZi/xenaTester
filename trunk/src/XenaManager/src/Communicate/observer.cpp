#include "observer.h"
#include "xenaTester.h"
#include <QTimer>
#include <QDebug>

CPortObserver::CPortObserver(CXenaTester* tester, QObject* parent/* = 0*/)
: QObject(parent)
, m_tester(tester)
, m_interval(500)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(observeProcess()));
}


CPortObserver::~CPortObserver()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

void CPortObserver::observePort(CPort *port)
{
    QMutexLocker _lock(&m_mutex);

    if (m_ports.contains(port)) {
        return ;
    }
    m_ports.push_back(port);
    if (!m_timer->isActive()) {
        m_timer->start(m_interval);
    }
}

void CPortObserver::removePort(CPort *port)
{
    QMutexLocker _lock(&m_mutex);

    if (m_ports.contains(port)) {
        m_ports.removeOne(port) ;
        if (m_ports.isEmpty()) {
            m_timer->stop();
        }
    }
}

void CPortObserver::removeAllPort()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }

    QMutexLocker _lock(&m_mutex);
    m_ports.clear();
}

void CPortObserver::setInterval(int ms)
{
    if (m_timer->isActive()) {
        m_timer->setInterval(ms);
    }
    m_interval = ms;
}

void CPortObserver::observeProcess()
{
    QMutexLocker _lock(&m_mutex);
    Q_FOREACH(CPort* port, m_ports)
    {
        if (!m_tester->portIsTrafficing(port)) {
            m_tester->portStopTranffic(port);
            if (!m_tester->portNextSpeedTraffic(port)) {
                qDebug() << "********* stop ************";
                _lock.unlock();
                removePort(port);
                _lock.relock();
                m_tester->portFinishedStrategy(port);
                emit taskFinished(port);
            }
            else {
                m_tester->portStartSpeedTraffic(port);
            }
        }
    }
}