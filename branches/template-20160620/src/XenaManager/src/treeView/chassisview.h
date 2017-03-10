/*  @file
 *  @brief ���˵���
 *  @author MSZ
 *  @date 2016/05/13
 *  @version 0.1
 */
#ifndef CHASSISVIEW_H
#define CHASSISVIEW_H

#include <QWidget>
#include <QModelIndex>
#include "XenaManager/src/ScriptCommand/scriptcommand.h"
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"
#include "XenaManager/src/dialog/ImportData/utilTools.h"
#include "XenaManager/src/XenaManageSettings/streamstrategysettings.h"

#define MAXCHASSISCOUNT 12
#define MAXPERCHASSISMODULECOUNT 12
#define MAXPERCHASSISPORTCOUNT 12

enum E_SELECTION_TYPE{
	CHASSISSELECTION,
	MODULESELECTION,
	PORTSELECTION,
	STREAMSELECTION,

	MAX_SELECTION
};

enum E_DATAPOSTION
{
	NAMEPOSITION,
	SYNCPOSTION,
	STATUSPOSTION,
	OWNERPOSITION
};

struct S_CHASSIS_INFO{
	int moduleCount;
	int portCount[MAXPERCHASSISMODULECOUNT];
	QString chassisName;
	QString moduleName[MAXPERCHASSISMODULECOUNT];
	QString portName[MAXPERCHASSISMODULECOUNT][MAXPERCHASSISPORTCOUNT];
};

struct S_UPDATE_INFO{
	E_SELECTION_TYPE type;
	QString name;
	int moduleIndex;
	int portIndex;
	int streamIndex;
};

struct S_CURRENT_POSION{
	E_SELECTION_TYPE currentType;
	int currentModulePosion;
	int currentPortPosion;
	int currentStreamPosion;
};

struct S_STREAM_STRATEGY{
	bool streamEnable;

	bool packetStrategyEnable;
	unsigned int packets;

	bool stepStrategyEnable;
	double beginRate;
	double endRate;
	double stepRate;
	unsigned int loopTimes;
	S_STREAM_STRATEGY()
	{
		streamEnable = StreamStrategySettings::getStreamEnable();

		packetStrategyEnable = StreamStrategySettings::getPacketStrategyEnable();
		packets = StreamStrategySettings::getPackets();

		stepStrategyEnable = StreamStrategySettings::getStepStrategyEnabel();
		beginRate = StreamStrategySettings::getBeginRate();
		endRate = StreamStrategySettings::getEndRate();
		stepRate = StreamStrategySettings::getStepRate();
		loopTimes = StreamStrategySettings::getLoopTimes();
	}
};

struct S_PORT_STRATEGY{
	bool timeStrategyEnable;
	unsigned int microseconds; 
	QList<S_STREAM_STRATEGY*>  streamStrategy;
};

// ������Ϣ����
typedef S_STREAM_STRATEGY StreamStrategy;
typedef S_PORT_STRATEGY PortStrategy;
typedef QList<PortStrategy*> ModuleStrategy;
typedef QList<ModuleStrategy*> ChassisStrategy;

// �����ݶ���
typedef ImportData StreamData;
typedef QList<StreamData*> PortData;
typedef QList<PortData*> ModuleData;
typedef QList<ModuleData*> ChassisData;

class QTreeView;
class QStandardItemModel;
class QAction;
class QMenu;
class Communicate;
class StateThread;
class ChassisView : public QWidget
{
	Q_OBJECT

public:
	explicit ChassisView(Communicate * communicate,QWidget *parent = 0);
	~ChassisView();
	
	void getChassisInfo();												// ��ȡ����Ļ�����Ϣ(��ʼ����ʱ��)

signals:
	void updateShow(S_UPDATE_INFO updateInfo);							// �����ͬ��treeViewʱ��Ӧ�������ý���
	void sendFrame(const ImportData & frames);							// ���ͽ������ļ������ݵ�segmentView��
	void updateTreeView(ImportData * importData);						// ���������ý����е�treeView�е�����
	void updatePortStrategy(PortStrategy * portStrategy);				// ���¶˿ڲ���
	void updateStreamStrategy(StreamStrategy * streamStrategy);			// ���������ý����еĲ���

private slots:
	void onReserveAction();												// ռ��ѡ�е��豸
	void onReleaseAction();												// �ͷ�ռ�õ��豸
	void onRelinquishAction();											// �����������豸ռ�õ��豸
	void onImportScdAction();											// ����SCD�ļ�����
	void onImportPcapAction();											// ����PCAP�ļ�����
	void onRemoveStreamAction();										// ɾ��ѡ�е���
	void onCustomContextMenuRequested(const QPoint &point);				// �Ҽ��˵�����
	void onViewClicked(QModelIndex index);								// ���chassisTreeView�Ҳ����ý���ͬ������
	void chassisName(QString & name);									// ��ȡ��������
	void moduleNameAndCount(int moduleIndex,QString & name);			// ��ȡ�忨���ƺ͸���
	void portCount(int moduleCount,const QStringList & numberList);		// ��ȡ�˿ڸ���
	void portName(int moduleIndex,int portIndex,QString & name);		// ��ȡ�˿�����
	void chassisReservedBy(QString & name);								// ��ȡռ�ö˿ڵ�����
	void moduleReservedBy(int moduleIndex,QString & name);				// ��ȡռ�ð忨������
	void portReservedBy(int moduleIndex,int portIndex,QString & name);	// ��ȡռ�ö˿ڵ�����
	void portTrafficStatus(int moduleIndex,int portIndex,const QString & trafficStatus); // �鿴�����˿��Ƿ����ڷ�������
	void portReceiveSync(int moduleIndex,int portIndex,const QString & syncStatus); // ��ȡ�����˿ڵ�ͬ��״̬ 

private:
	void initUI();
	void setMainLayout();
	void createActions();												// �����µĶ���ָ��
	void createContextMenu();											// �����ʼ��˵�

	void setTreeViewData();												// ����treeView������(��ʼ����ʱ�������������)

	void startThread();													// ����״̬��ѯ����

	void getCurrentPortPositon(S_ORDER & order);						// ��ȡ��ǰ�˿�λ����Ϣ
	void getCurrentStreamPosition(S_ORDER & order);						// ��ȡ��ǰ����λ����Ϣ

	void importFile(const QString & fileName,IMPORT_FILE_STYLE style);	// ����SCD����PCAP�ļ���������
	bool addStream(int module, int port, StreamData* streamData);		// ��������Ϣ
	void clearChassisData();											// ���������Ϣ  ������ʱ������������Ϣ�ֶ�ɾ��

	void getCurrentStreamPostion(QModelIndex & index);					// ��ȡ��ǰѡ�е�����λ��
	StreamData* getCurrentStream();										// ��ȡ��ǰѡ�е���
	S_ORDER getCurrentOrderInfo();										// ����ǰ��Ϣ���ӵ�ָ��������
	QModelIndex getCurrentStreamModuleIndex(int moduleIndex,int portIndex,E_DATAPOSTION postion); // ��ȡ�˿ڵ�ModuleIndex
	QStandardItem* getCurrentStreamIndex();								// ��ȡ��ǰStream��Item
	void setSiblingDataRole(QStandardItem *standardItem,E_SELECTION_TYPE role,int index = 0); // ���ó������������Item�Ľ�ɫ

	void addStreamStrategy(int module, int port, StreamStrategy* streamStrategy);// ���������Ե��˿ڲ�����
	void clearChassisStrategy();										//  ����������  ������ʱ�������Ļ��������Ϣ�ֶ�ɾ��	
	PortStrategy * getCurrentPortStrategy();
	StreamStrategy * getCurrentStreamStrategy();

private:
	S_CHASSIS_INFO m_chassisInfo;
	S_CURRENT_POSION m_currenPosion;
	QTreeView *m_pChassisTreeView;
	QStandardItemModel *m_pChassisModel;
	QAction *m_pImportScdAction;
	QAction *m_pImportPcapAction;
	QAction *m_pReserveAction;
	QAction *m_pReleaseAction;
	QAction *m_pRelinquishAction;
	QAction *m_pRemoveStreamAction;
	Communicate *m_pCommunicate;
	StateThread *m_pStateThread;

	ChassisData m_chassisData;											// ����Ϣ

	ChassisStrategy m_chassisStrategy;									// ���������͵Ĳ�����Ϣ
	bool m_inited;
};

#endif // CHASSISVIEW_H
