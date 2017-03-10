#ifndef CHASSISVIEWWIDGET_H
#define CHASSISVIEWWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"

// ���䡢�忨���˿ں����ֱ��Ӧ��ѡ���ɫ
enum E_SELECTION_TYPE{
	CHASSISSELECTION,
	MODULESELECTION,
	PORTSELECTION,
	STREAMSELECTION,

	MAX_SELECTION
};

// treeView�е�λ��
enum E_COLUMN_INDEX
{
	NAMEINDEX,
	SYNCINDEX,
	STATUSINDEX,
	OWNERINDEX
};

//  ��굱ǰѡ�е�λ��
struct S_CURRENT_INDEX{
	E_SELECTION_TYPE currentType;
	int currentModuleIndex;
	int currentPortIndex;
	int currentStreamIndex;
};

// �����ݶ���
// typedef ImportData StreamData;
// typedef QList<StreamData*> PortData;
// typedef QList<PortData*> ModuleData;
// typedef QList<ModuleData*> ChassisData;


class CXenaTester;
class IDevice;
class CChassis;
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

	void initTreeViewWidgetData();										// ����treeView������(��ʼ����ʱ�������������)

signals:
	void updateConfigWidget(S_CURRENT_INDEX m_currenIndex);				// ���ѡ��������Ҳ����ô���
    void updatePortTraffic(int moduleIndex,int portIndex);              // ���¶˿ڷ���״̬

private slots:
	void onReserveAction();												// ռ��ѡ�е��豸
	void onReleaseAction();												// �ͷ�ռ�õ��豸
	void onRelinquishAction();											// �����������豸ռ�õ��豸
	void onImportScdAction();											// ����SCD�ļ�����
	void onImportPcapAction();											// ����PCAP�ļ�����
	void onRemoveStreamAction();										// ɾ��ѡ�е���
	void onCustomContextMenuRequested(const QPoint &point);				// �Ҽ��˵�����
	void onViewClicked(QModelIndex index);								// ���chassisTreeView�Ҳ����ý���ͬ������

	void updateChassisStatus();                                         // ��ʱ���»�����Ϣ

private:
	void initUI();
	void setMainLayout();
	void createActions();                                               // �����µĶ���ָ��
	void createContextMenu();                                           // �����Ҽ��˵�

    void startUpdate();                                                 // ����״̬��ѯ��ʱ��

    void importFile(const QString & fileName,IMPORT_FILE_STYLE style);  // ����SCD����PCAP�ļ���������

    ///////
    QStandardItem* getCurrentPortItem();                                // ��ȡ��ǰ�˿ڵ�StandardItem
    QModelIndex getCurrentPortModelIndex(int moduleIndex,int portIndex);// ��ȡ��ǰ�˿ڵ�ModelIndex
    void getCurrentStreamIndex(QModelIndex & index);                    // ��ȡ��ǰ��������

    void updateChassisOwner();                                          // ���»���ռ�����
    void updateModuleOwner(int moduleIndex);                            // ���°忨ռ�����
    void updatePortName(int moduleIndex,int portIndex);                 // ���¶˿�ռ��״̬
    void updatePortCommunicationStatus(int moduleIndex,int portIndex);  // ���¶˿�ͬ���ͷ���״̬
    void updatePortOwner(int moduleIndex,int portIndex);                // ���¶˿�ռ�����
    void setSiblingDataRole(QStandardItem *standardItem,E_SELECTION_TYPE role,int index); // ���ó������������Item�Ľ�ɫ

	
private:
    CXenaTester				*m_pXenaTester;
    CChassis				*m_pChassis;
    IDevice                 *m_pDevice;

    S_CURRENT_INDEX			m_currenIndex;

    QTimer                  *m_pUpdateStatusTimer;

    QTreeView				*m_pChassisTreeView;
    QStandardItemModel		*m_pChassisModel;

    QAction					*m_pImportScdAction;
    QAction					*m_pImportPcapAction;
    QAction					*m_pReserveAction;
    QAction					*m_pReleaseAction;
    QAction					*m_pRelinquishAction;
    QAction					*m_pRemoveStreamAction;
};

#endif // CHASSISVIEWWIDGET_H
