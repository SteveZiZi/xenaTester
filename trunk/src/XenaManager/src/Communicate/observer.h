#pragma once

#include <QObject>
#include <QList>
#include <QMutex>

class CPort;
class QTimer;
class CXenaTester;

class CPortObserver : public QObject
{
    Q_OBJECT
public:
    CPortObserver(CXenaTester* tester, QObject* parent = 0);
    ~CPortObserver();

public:
    void observePort(CPort *port);
    void removePort(CPort *port);
    void removeAllPort();

    void setInterval(int ms);

signals:
    void taskFinished(CPort *port);


protected slots:
    void observeProcess();

private:
    QList<CPort*> m_ports;
    CXenaTester *m_tester;

    QTimer *m_timer;
    int m_interval;

    QMutex m_mutex;
};