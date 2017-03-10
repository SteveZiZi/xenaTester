#ifndef STATETHREAD_H
#define STATETHREAD_H

#include <QThread>
#include <QMutex>

#define MAXMODULECOUNT 12

class Communicate;
class QTimer;
class StateThread : public QThread
{
	Q_OBJECT

public:
	StateThread(Communicate *communicate,QObject *parent = 0);
	~StateThread();
	void start();
	void getModuleCount(int moduleCount);
	void getPortCount(int moduleIndex,int portNum);

private slots:
	void run();

private:
	void stop();
private:
	QTimer *m_pUpdateTimer;
	QMutex m_mutex;
	//Communicate *m_pCommunicate;
	int m_moduleCount;
	int m_portCount[MAXMODULECOUNT];
};

#endif // STATETHREAD_H
