#ifndef CHASSISVIEWWIDGET_H
#define CHASSISVIEWWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"
#include "XenaManager/src/dialog/modifierDialog/modifierDialog.h"

// 机箱、板卡、端口和流分别对应的选项角色
enum E_SELECTION_TYPE{
	CHASSISSELECTION,
	MODULESELECTION,
	PORTSELECTION,
	STREAMSELECTION,

	MAX_SELECTION
};

// treeView列的位置
enum E_COLUMN_INDEX
{
	NAMEINDEX,
	SYNCINDEX,
	STATUSINDEX,
	OWNERINDEX
};

//  鼠标当前选中的位置
struct S_CURRENT_INDEX{
	E_SELECTION_TYPE currentType;
	int currentModuleIndex;
	int currentPortIndex;
	int currentStreamIndex;
};

// 流数据定义
// typedef ImportData StreamData;
// typedef QList<StreamData*> PortData;
// typedef QList<PortData*> ModuleData;
// typedef QList<ModuleData*> ChassisData;



class CXenaTester;
class IDevice;
class CChassis;
class CPort;
class CStream;
class UpdateTrafficStatus;
class QTreeView;
class QStandardItemModel;
class QAction;
class QMenu;
class QTimer;
class QStandardItem;
class ChassisViewWidget : public QWidget
{
	Q_OBJECT

public:
	ChassisViewWidget(CXenaTester *xenaTester,QWidget *parent = 0);
	~ChassisViewWidget();

	void initTreeViewWidgetData();										// 设置treeView的数据(初始化的时候进行数据设置)

signals:
	void updateConfigWidget(S_CURRENT_INDEX m_currenIndex);				// 点击选中项更新右侧配置窗口
    void updatePortTraffic(int moduleIndex,int portIndex);              // 更新端口发送状态

private slots:
	void onReserveAction();												// 占用选中的设备
	void onReleaseAction();												// 释放占用的设备
	void onRelinquishAction();											// 放弃被其他设备占用的设备
	void onImportScdAction();											// 导入SCD文件功能
	void onImportPcapAction();											// 导入PCAP文件功能
	void onRemoveStreamAction();										// 删掉选中的流
    void onImportAction();                                              // 导入配置文件功能
    void onExportAction();                                              // 导出配置文件功能
	void onCustomContextMenuRequested(const QPoint &point);				// 右键菜单功能
	void onViewClicked(QModelIndex index);								// 点击chassisTreeView右侧配置界面同步跟新

	void updateChassisStatus();                                         // 定时更新机箱信息
    void onKeepAliveTimerOut();

private:
	void initUI();
	void setMainLayout();
	void createActions();                                               // 创建新的动作指令
	void createContextMenu();                                           // 创建右键菜单

    void startUpdate();                                                 // 开启状态查询定时器

    void importFile(const QString & fileName,IMPORT_FILE_STYLE style);  // 导入SCD或者PCAP文件并生成流

    ///////
    QStandardItem* getCurrentPortItem();                                // 获取当前端口的StandardItem
    QStandardItem* getPortItem(int moduleIndex, int portIndex);         // 根据索引获取对应的PortItem
    QModelIndex getCurrentPortModelIndex(int moduleIndex,int portIndex);// 获取当前端口的ModelIndex
    void getCurrentStreamIndex(QModelIndex & index);                    // 获取当前流的索引

    void updateChassisOwner();                                          // 更新机箱占用情况
    void updateModuleOwner(int moduleIndex);                            // 更新板卡占用情况
    void updatePortName(int moduleIndex,int portIndex);                 // 更新端口占用状态
    void updatePortCommunicationStatus(int moduleIndex,int portIndex);  // 更新端口同步和发送状态
    void updatePortOwner(int moduleIndex,int portIndex);                // 更新端口占用情况
    void setSiblingDataRole(QStandardItem *standardItem,E_SELECTION_TYPE role,int index); // 设置除名称外的所有Item的角色

    void getPortConfigInfo(int moduleIndex, int portIndex);

    // 添加流和删除流接口
    bool addStream(int moduleIndex, int portIndex, ImportData* importData);
    bool addStream(int moduleIndex, int portIndex, CStream *stream);
    bool addStreams(int mIndex, int pIndex, CPort *port);

    bool delStream(int moduleIndex, int portIndex, int streamIndex);
    bool delStream(CStream *stream);

    bool addMidifier(int moduleIndex, int PortIndex, int streamIndex , int modifierIndex, CModifierDialog::ModifierData * modifierData);

    // 导入配置文件后配置端口
    bool configPort(int moduleIndex, int portIndex, CPort * port);
    bool configStream(int moduleIndex, int portIndex, int streamIndex, CStream *stream_);

    bool onImportPortConf(int moduleIndex, int portIndex, QString fileName);
    bool onImportModuleConf(int moduleIndex, QString fileName);

	
private:
    CXenaTester				*m_pXenaTester;
    CChassis				*m_pChassis;

    S_CURRENT_INDEX			m_currenIndex;

    QTimer                  *m_pUpdateStatusTimer;
    QTimer                  *m_pKeepAliveTimer;

    QTreeView				*m_pChassisTreeView;
    QStandardItemModel		*m_pChassisModel;

    QAction					*m_pImportScdAction;
    QAction					*m_pImportPcapAction;
    QAction					*m_pReserveAction;
    QAction					*m_pReleaseAction;
    QAction					*m_pRelinquishAction;
    QAction					*m_pRemoveStreamAction;
    QAction                 *m_pImportAction;							// 导入配置
    QAction                 *m_pExportAction;							// 导出配置
};

#endif // CHASSISVIEWWIDGET_H
