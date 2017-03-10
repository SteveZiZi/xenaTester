/*  @file
 *  @brief 左侧菜单类
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

// 策略信息定义
typedef S_STREAM_STRATEGY StreamStrategy;
typedef S_PORT_STRATEGY PortStrategy;
typedef QList<PortStrategy*> ModuleStrategy;
typedef QList<ModuleStrategy*> ChassisStrategy;

// 流数据定义
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
	
	void getChassisInfo();												// 获取机箱的基本信息(初始化的时候)

signals:
	void updateShow(S_UPDATE_INFO updateInfo);							// 点击不同的treeView时对应更新配置界面
	void sendFrame(const ImportData & frames);							// 发送解析的文件的内容到segmentView中
	void updateTreeView(ImportData * importData);						// 更新流配置界面中的treeView中的内容
	void updatePortStrategy(PortStrategy * portStrategy);				// 更新端口策略
	void updateStreamStrategy(StreamStrategy * streamStrategy);			// 更新流配置界面中的策略

private slots:
	void onReserveAction();												// 占用选中的设备
	void onReleaseAction();												// 释放占用的设备
	void onRelinquishAction();											// 放弃被其他设备占用的设备
	void onImportScdAction();											// 导入SCD文件功能
	void onImportPcapAction();											// 导入PCAP文件功能
	void onRemoveStreamAction();										// 删掉选中的流
	void onCustomContextMenuRequested(const QPoint &point);				// 右键菜单功能
	void onViewClicked(QModelIndex index);								// 点击chassisTreeView右侧配置界面同步跟新
	void chassisName(QString & name);									// 获取机箱名称
	void moduleNameAndCount(int moduleIndex,QString & name);			// 获取板卡名称和个数
	void portCount(int moduleCount,const QStringList & numberList);		// 获取端口个数
	void portName(int moduleIndex,int portIndex,QString & name);		// 获取端口名称
	void chassisReservedBy(QString & name);								// 获取占用端口的名称
	void moduleReservedBy(int moduleIndex,QString & name);				// 获取占用板卡的名称
	void portReservedBy(int moduleIndex,int portIndex,QString & name);	// 获取占用端口的名称
	void portTrafficStatus(int moduleIndex,int portIndex,const QString & trafficStatus); // 查看各个端口是否正在发送数据
	void portReceiveSync(int moduleIndex,int portIndex,const QString & syncStatus); // 获取各个端口的同步状态 

private:
	void initUI();
	void setMainLayout();
	void createActions();												// 创建新的动作指令
	void createContextMenu();											// 创建邮件菜单

	void setTreeViewData();												// 设置treeView的数据(初始化的时候进行数据设置)

	void startThread();													// 启动状态查询进程

	void getCurrentPortPositon(S_ORDER & order);						// 获取当前端口位置信息
	void getCurrentStreamPosition(S_ORDER & order);						// 获取当前流的位置信息

	void importFile(const QString & fileName,IMPORT_FILE_STYLE style);	// 导入SCD或者PCAP文件并生成流
	bool addStream(int module, int port, StreamData* streamData);		// 增加流信息
	void clearChassisData();											// 清除机箱信息  在析构时将创建的流信息手动删除

	void getCurrentStreamPostion(QModelIndex & index);					// 获取当前选中的流的位置
	StreamData* getCurrentStream();										// 获取当前选中的流
	S_ORDER getCurrentOrderInfo();										// 将当前信息附加到指令内容中
	QModelIndex getCurrentStreamModuleIndex(int moduleIndex,int portIndex,E_DATAPOSTION postion); // 获取端口的ModuleIndex
	QStandardItem* getCurrentStreamIndex();								// 获取当前Stream的Item
	void setSiblingDataRole(QStandardItem *standardItem,E_SELECTION_TYPE role,int index = 0); // 设置除名称外的所有Item的角色

	void addStreamStrategy(int module, int port, StreamStrategy* streamStrategy);// 增加流策略到端口策略中
	void clearChassisStrategy();										//  清除机箱策略  在析构时将创建的机箱策略信息手动删除	
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

	ChassisData m_chassisData;											// 流信息

	ChassisStrategy m_chassisStrategy;									// 数据流发送的策略信息
	bool m_inited;
};

#endif // CHASSISVIEW_H
