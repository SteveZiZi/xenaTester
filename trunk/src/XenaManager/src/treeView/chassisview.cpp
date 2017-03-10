#include "chassisview.h"
#include "XenaManager/src/Communicate/communicate.h"
#include "XenaManager/src/StateManagement/statethread.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include "XenaManager/src/XenaManageSettings/streamstrategysettings.h"
#include <QAction>
#include <QMenu>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDebug>

#define MAXWIDGETWIDTH 520
#define QUOTATIONMARK "\""
#define MAXPACKETLENGTH "1024"

ChassisView::ChassisView(Communicate * communicate,QWidget *parent)
	: QWidget(parent),
	m_pCommunicate(communicate),
	m_inited(false)	
{
	m_chassisInfo.moduleCount = 0;
	initUI();

	connect(m_pChassisTreeView,SIGNAL(clicked(QModelIndex)),this,SLOT(onViewClicked(QModelIndex)));
	connect(m_pCommunicate,SIGNAL(chassisName(QString &)),this,SLOT(chassisName(QString &)));
	connect(m_pCommunicate,SIGNAL(moduleNameAndCount(int,QString &)),this,SLOT(moduleNameAndCount(int,QString &)));
	connect(m_pCommunicate,SIGNAL(portCount(int,const QStringList &)),this,SLOT(portCount(int,const QStringList &)));
	connect(m_pCommunicate,SIGNAL(portName(int,int,QString &)),this,SLOT(portName(int,int,QString &)));
	connect(m_pCommunicate,SIGNAL(chassisReservedBy(QString &)),this,SLOT(chassisReservedBy(QString &)));
	connect(m_pCommunicate,SIGNAL(moduleReservedBy(int,QString &)),this,SLOT(moduleReservedBy(int,QString &)));
	connect(m_pCommunicate,SIGNAL(portReservedBy(int,int,QString &)),this,SLOT(portReservedBy(int,int,QString &)));
	connect(m_pCommunicate,SIGNAL(portTrafficStatus(int,int,const QString &)),this,SLOT(portTrafficStatus(int,int,const QString &)));
	connect(m_pCommunicate,SIGNAL(portReceiveSync(int,int,const QString &)),this,SLOT(portReceiveSync(int,int,const QString &)));
}

ChassisView::~ChassisView()
{
	m_pStateThread->wait();
	clearChassisData();
	clearChassisStrategy();
}

void ChassisView::getChassisInfo()
{
	S_ORDER order;
	m_pCommunicate->sendData(C_NAME,order);								// ��ѯ��������
	m_pCommunicate->sendData(M_MODULE_COUNTS,order);					// ��ѯ�忨����������
	m_pCommunicate->sendData(C_PORTCOUNTS,order);						// ��ѯ�˿�����
}

void ChassisView::onReserveAction()
{
	switch(m_currenPosion.currentType)
	{
	case CHASSISSELECTION:
		{
			S_ORDER order;
			m_pCommunicate->sendData(C_RESERVATION_RESERVE,order);
			m_pCommunicate->sendData(C_RESERVEDBY,order);				// ������Ӧռ��
			break;
		}
	case MODULESELECTION:
		{
			S_ORDER moduleOrder;
			moduleOrder.moduleIndex = m_currenPosion.currentModulePosion;
			m_pCommunicate->sendData(M_RESERVATION_RESERVE,moduleOrder);
			m_pCommunicate->sendData(M_RESERVEDBY,moduleOrder);			// ������Ӧռ��״̬
			break;
		}
	case PORTSELECTION:
		{
			S_ORDER portOrder;
			getCurrentPortPositon(portOrder);
			m_pCommunicate->sendData(P_RESERVATION_RESERVE,portOrder);
			m_pCommunicate->sendData(P_RESERVEDBY,portOrder);			// ������Ӧռ��״̬

			portOrder.data = MAXPACKETLENGTH;
			m_pCommunicate->sendData(P_MAXHEADERLENGTH_SET,portOrder);	// ����ÿ���˿ڵ����ͷ����Ϊ1024(���ɱ༭�ĳ���)

			m_pCommunicate->sendData(P_MAXHEADERLENGTH_QUERY,portOrder);// ��ѯ�˿�֡ͷ���ȣ�ʹ������������
		}
	default:
		break;
	}
}

void ChassisView::onReleaseAction()
{
	switch(m_currenPosion.currentType)
	{
	case CHASSISSELECTION:
		{
			S_ORDER order;
			m_pCommunicate->sendData(C_RESERVATION_RELEASE,order);
			m_pCommunicate->sendData(C_RESERVEDBY,order);				// ������Ӧռ��
			break;
		}
	case MODULESELECTION:
		{
			S_ORDER moduleOrder;
			moduleOrder.moduleIndex = m_currenPosion.currentModulePosion;
			m_pCommunicate->sendData(M_RESERVATION_RELEASE,moduleOrder);
			m_pCommunicate->sendData(M_RESERVEDBY,moduleOrder);			// ������Ӧռ��״̬
			break;
		}
	case PORTSELECTION:
		{
			S_ORDER portOrder;
			getCurrentPortPositon(portOrder);
			m_pCommunicate->sendData(P_RESERVATION_RELEASE,portOrder);
			m_pCommunicate->sendData(P_RESERVEDBY,portOrder);			// ������Ӧռ��״̬
		}
	default:
		break;
	}
}

void ChassisView::onRelinquishAction()
{
	switch(m_currenPosion.currentType)
	{
	case CHASSISSELECTION:
		{
			S_ORDER order;
			m_pCommunicate->sendData(C_RESERVATION_RELINQUISH,order);
			m_pCommunicate->sendData(C_RESERVEDBY,order);				// ������Ӧռ��
			break;
		}
	case MODULESELECTION:
		{
			S_ORDER moduleOrder;
			moduleOrder.moduleIndex = m_currenPosion.currentModulePosion;
			m_pCommunicate->sendData(M_RESERVATION_RELINQUISH,moduleOrder);
			m_pCommunicate->sendData(M_RESERVEDBY,moduleOrder);			// ������Ӧռ��״̬
			break;
		}
	case PORTSELECTION:
		{
			S_ORDER portOrder;
			getCurrentPortPositon(portOrder);
			m_pCommunicate->sendData(P_RESERVATION_RELINQUISH,portOrder);
			m_pCommunicate->sendData(P_RESERVEDBY,portOrder);			// ������Ӧռ��״̬
		}
	default:
		break;
	}
}

void ChassisView::onImportScdAction()
{
	S_ORDER portOrder;
	getCurrentPortPositon(portOrder);

	QString scdFileName = QFileDialog::getOpenFileName(0,tr("��SCD�ļ�"),QString(),
		tr("SCL (*.scd *.icd *.cid);; �����ļ�(*.*)"));

	importFile(scdFileName,IMPORT_SCD_FILE);
}

void ChassisView::onImportPcapAction()
{
	S_ORDER portOrder;
	getCurrentPortPositon(portOrder);

	QString pcapFileName = QFileDialog::getOpenFileName(0,tr("��PCAP�ļ�"),QString(),
		tr("PCAP (*.pcap);; �����ļ�(*.*)"));

	importFile(pcapFileName,IMPORT_PCAP_FILE);
}

void ChassisView::onRemoveStreamAction()
{
	// ɾ������
	ModuleData* moduleData = m_chassisData.at(m_currenPosion.currentModulePosion);
	PortData* portData = moduleData->at(m_currenPosion.currentPortPosion);
	StreamData *streamData = portData->at(m_currenPosion.currentStreamPosion);
	delete streamData;
	portData->takeAt(m_currenPosion.currentStreamPosion);

	// ɾ��������
	QStandardItem *portItem = getCurrentStreamIndex();
	portItem->removeRow(m_currenPosion.currentStreamPosion);

	// ɾ����ָ��
	S_ORDER order = getCurrentOrderInfo();
	m_pCommunicate->sendData(PS_DELETE,order);
}

void ChassisView::onCustomContextMenuRequested(const QPoint &point)
{
	QModelIndex index = m_pChassisTreeView->indexAt(point);
	QVariant var = index.data(Qt::UserRole + 1);
	if (!var.isValid())
	{
		return;
	}

	QMenu contextMenu;
	m_currenPosion.currentType =  static_cast<E_SELECTION_TYPE>(var.toInt());
	switch(var.toInt())
	{
	case CHASSISSELECTION:
		contextMenu.addAction(m_pReserveAction);
		contextMenu.addAction(m_pReleaseAction);
		contextMenu.addAction(m_pRelinquishAction);
		break;
	case MODULESELECTION:
		m_currenPosion.currentModulePosion = index.row();
		contextMenu.addAction(m_pReserveAction);
		contextMenu.addAction(m_pReleaseAction);
		contextMenu.addAction(m_pRelinquishAction);
		break;
	case PORTSELECTION:
		m_currenPosion.currentModulePosion = index.parent().row();
		m_currenPosion.currentPortPosion = index.row();
		contextMenu.addAction(m_pReserveAction);
		contextMenu.addAction(m_pReleaseAction);
		contextMenu.addAction(m_pRelinquishAction);
		contextMenu.addSeparator();
		contextMenu.addAction(m_pImportScdAction);
		contextMenu.addAction(m_pImportPcapAction);
		break;
	case STREAMSELECTION:
		{
			getCurrentStreamPostion(index);
			contextMenu.addAction(m_pRemoveStreamAction);

			StreamData *streamData = getCurrentStream();
			emit updateTreeView(streamData);
			break;
		}
	default:
		break;
	}
	contextMenu.exec(QCursor::pos());  
}

void ChassisView::onViewClicked(QModelIndex index)
{
	S_UPDATE_INFO updateInfo;
	if (index.column() > 0)
	{
		index = index.sibling(index.row(),0);
	}

	QStandardItem *item = m_pChassisModel->itemFromIndex(index);
	updateInfo.name = item->text();

	QVariant var = index.data(Qt::UserRole + 1);
	if (!var.isValid())
	{
		qDebug() << "======ChassisView::onViewClicked::position false ======";
		return;
	}

	switch(var.toInt())
	{
	case CHASSISSELECTION:
		break;
	case MODULESELECTION:
		m_currenPosion.currentModulePosion = index.row();
		break;
	case PORTSELECTION:
		{
			m_currenPosion.currentModulePosion = index.parent().row();
			m_currenPosion.currentPortPosion = index.row();

			S_ORDER order;
			getCurrentPortPositon(order);
			m_pCommunicate->sendData(P_TRAFFIC_QUERY,order);			// ���¶˿ڵĴ���״̬
			m_pCommunicate->sendData(P_MAXHEADERLENGTH_QUERY,order);	// ���¶˿ڵ�֡ͷ����
			m_pCommunicate->sendData(P_TXTIMELIMIT_QUERY,order);		// ���¶˿ڵı���ʱ��

			PortStrategy *portStrategy =  getCurrentPortStrategy();
			emit updatePortStrategy(portStrategy);						// ����˿ڸ��¶˿ڲ���
			break;
		}
	case STREAMSELECTION:
		{
			getCurrentStreamPostion(index);

			StreamData *streamData = getCurrentStream();
			emit updateTreeView(streamData);
			
			S_ORDER streamOrder;
			getCurrentStreamPosition(streamOrder);

			m_pCommunicate->sendData(PS_ENABLE_QUERY,streamOrder);		// ���������ý����е�֡�Ƿ�ʹ��
			m_pCommunicate->sendData(PS_PACKETLIMIT_QUERY,streamOrder);	// ���������ý����е�֡��������
			m_pCommunicate->sendData(PS_RATEFRACTION_QUERY,streamOrder);// ���������ý����е�֡����(�ٷֱ�)
			m_pCommunicate->sendData(PS_RATEPPS_QUERY,streamOrder);		// ���������ý����е�֡����(֡ÿ��)

			m_pCommunicate->sendData(P_SPEED,streamOrder);				// ��ȡ��Ӧ�˿ڵ�����

			StreamStrategy *streamStrategy = getCurrentStreamStrategy();
			emit updateStreamStrategy(streamStrategy);
			break;
		}
	default:
		break;
	}
	m_currenPosion.currentType =  static_cast<E_SELECTION_TYPE>(var.toInt());
	updateInfo.type = static_cast<E_SELECTION_TYPE>(var.toInt());
	updateInfo.moduleIndex = m_currenPosion.currentModulePosion;
	updateInfo.portIndex = m_currenPosion.currentPortPosion;
	updateInfo.streamIndex = m_currenPosion.currentStreamPosion;

	emit updateShow(updateInfo);	
}

void ChassisView::chassisName(QString & name)
{
	name.remove(QUOTATIONMARK);
	m_chassisInfo.chassisName = name;
}

void ChassisView::moduleNameAndCount(int moduleIndex,QString & name)
{
	if (name.isEmpty())
	{
		name = "Bad Module";
	}
	name.remove(QUOTATIONMARK);
	if (m_chassisInfo.moduleCount + 1 != moduleIndex + 1)
	{
		m_chassisInfo.moduleName[m_chassisInfo.moduleCount] = "Bad Module";
	}
	m_chassisInfo.moduleCount = moduleIndex + 1;
	m_chassisInfo.moduleName[moduleIndex] = "Module" + QString::number(moduleIndex) + " " + name;
}

void ChassisView::portCount(int moduleCount,const QStringList & numberList)
{
	for (int i = 0;i < moduleCount;i++)
	{
		m_chassisInfo.portCount[i] = numberList[i].toInt();
	}

	for (int module = 0;module < moduleCount;module++)
	{
		for (int port = 0;port < m_chassisInfo.portCount[module];port++)
		{
			m_chassisInfo.portName[module][port] = "Bad Port";			// �ȳ�ʼ���˿���Ϊbad port �Է������Ķ˿ڳ���
			S_ORDER order;
			order.moduleIndex = module;
			order.portIndex = port;
			m_pCommunicate->sendData(P_INTERFACE,order);
		}
	}
}

void ChassisView::portName(int moduleIndex,int portIndex,QString & name)
{
	if (name.isEmpty())
	{
		name = "Bad Port";
	}
	name.remove(QUOTATIONMARK);
	QString portName = "Port " + QString::number(portIndex) + " " + name;
	m_chassisInfo.portName[moduleIndex][portIndex] = portName;
	// ��ʼ������  ���忨������ѯ����󣬲��˿�������ѯ�����ſ�ʼ��ʼ��
	if (!m_inited)
	{
		if (moduleIndex == m_chassisInfo.moduleCount - 1)
		{
			if (portIndex == m_chassisInfo.portCount[moduleIndex] - 1)
			{
				setTreeViewData();
				m_inited = true;
			}
		}
	}
	// ��ʼ�����ѯ�忨����
	else
	{
		QModelIndex portChildIndex = getCurrentStreamModuleIndex(moduleIndex,portIndex,NAMEPOSITION);
		m_pChassisModel->setData(portChildIndex,portName);
	}
}

void ChassisView::chassisReservedBy(QString & name)
{
	name.remove(QUOTATIONMARK);
	QModelIndex chassisOwnerIndex = m_pChassisModel->index(0,OWNERPOSITION,QModelIndex());
	m_pChassisModel->setData(chassisOwnerIndex,name);
}

void ChassisView::moduleReservedBy(int moduleIndex,QString & name)
{
	name.remove(QUOTATIONMARK);
	QModelIndex chassisParentIndex = m_pChassisModel->index(0,0,QModelIndex());
	QModelIndex moduleChildIndex = m_pChassisModel->index(moduleIndex,OWNERPOSITION,chassisParentIndex);
	m_pChassisModel->setData(moduleChildIndex,name);
}

void ChassisView::portReservedBy(int moduleIndex,int portIndex,QString & name)
{
	name.remove(QUOTATIONMARK);
	QModelIndex portChildIndex = getCurrentStreamModuleIndex(moduleIndex,portIndex,OWNERPOSITION);
	m_pChassisModel->setData(portChildIndex,name);
}

void ChassisView::portTrafficStatus(int moduleIndex,int portIndex,const QString & trafficStatus)
{
	QModelIndex portChildIndex = getCurrentStreamModuleIndex(moduleIndex,portIndex,STATUSPOSTION);		
	m_pChassisModel->setData(portChildIndex,trafficStatus);
}

void ChassisView::portReceiveSync(int moduleIndex,int portIndex,const QString & syncStatus)
{
	QString sync;
	if (syncStatus.contains("IN_SYNC",Qt::CaseInsensitive))
	{
		sync = "YES";
	}
	else if (syncStatus.contains("NO_SYNC",Qt::CaseInsensitive))
	{
		sync = "NO";
	}
	QModelIndex portChildIndex = getCurrentStreamModuleIndex(moduleIndex,portIndex,SYNCPOSTION);		
	m_pChassisModel->setData(portChildIndex,sync);
}

void ChassisView::initUI()
{
	// for test
// 	m_chassisInfo.moduleCount = 2;
// 	m_chassisInfo.portCount[0] = 6;
// 	m_chassisInfo.portCount[1] = 6;
// 	m_chassisInfo.chassisName = "New Chassis";
// 	m_chassisInfo.moduleName[0] = "Module 0";
// 	m_chassisInfo.moduleName[1] = "Module 1";
// 	m_chassisInfo.portName[0][0] = "port 0";
// 	m_chassisInfo.portName[0][1] = "port 1";
// 	m_chassisInfo.portName[0][2] = "port 2";
// 	m_chassisInfo.portName[0][3] = "port 3";
// 	m_chassisInfo.portName[0][4] = "port 4";
// 	m_chassisInfo.portName[0][5] = "port 5";
// 
// 	m_chassisInfo.portName[1][0] = "port 0";
// 	m_chassisInfo.portName[1][1] = "port 1";
// 	m_chassisInfo.portName[1][2] = "port 2";
// 	m_chassisInfo.portName[1][3] = "port 3";
// 	m_chassisInfo.portName[1][4] = "port 4";
// 	m_chassisInfo.portName[1][5] = "port 5";

	m_currenPosion.currentModulePosion = 0;
	m_currenPosion.currentPortPosion = 0;
	m_currenPosion.currentStreamPosion = 0;

	m_pChassisTreeView = new QTreeView;
	m_pChassisTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pChassisTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pChassisTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
	m_pChassisModel = new QStandardItemModel(this);
	m_pChassisTreeView->setModel(m_pChassisModel);
	QStringList chassisStringList;
	chassisStringList<< tr("����") << tr("ͬ��") << tr("״̬") << tr("ռ����");
	m_pChassisModel->setHorizontalHeaderLabels(chassisStringList);

	m_pStateThread = new StateThread(m_pCommunicate,this);				// ��������chassisռ��״̬�߳�

	createContextMenu();
	setMainLayout();
}

void ChassisView::setMainLayout()
{
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(m_pChassisTreeView);
	mainLayout->setMargin(1);

	this->setLayout(mainLayout);
	this->setMaximumWidth(MAXWIDGETWIDTH);
}

void ChassisView::createActions()
{
	m_pReserveAction = new QAction(tr("ռ��"),this);
	m_pReserveAction->setIcon(QIcon(":/images/icon_occupy.png"));
	connect(m_pReserveAction,SIGNAL(triggered()),this,SLOT(onReserveAction()));

	m_pReleaseAction = new QAction(tr("�ͷ�"),this);
	m_pReleaseAction->setIcon(QIcon(":/images/icon_release.png"));
	connect(m_pReleaseAction,SIGNAL(triggered()),this,SLOT(onReleaseAction()));

	m_pRelinquishAction = new QAction(tr("ע��"),this);
	m_pRelinquishAction->setIcon(QIcon(":/images/icon_Reset.png"));
	connect(m_pRelinquishAction,SIGNAL(triggered()),this,SLOT(onRelinquishAction()));

	m_pImportScdAction = new QAction(tr("������(SCD)"),this);
	m_pImportScdAction->setIcon(QIcon(":/images/icon_Import_SCD.png"));
	connect(m_pImportScdAction,SIGNAL(triggered()),this,SLOT(onImportScdAction()));

	m_pImportPcapAction = new QAction(tr("������(PCAP)"),this);
	m_pImportPcapAction->setIcon(QIcon(":/images/icon_Import_PCAP.png"));
	connect(m_pImportPcapAction,SIGNAL(triggered()),this,SLOT(onImportPcapAction()));

	m_pRemoveStreamAction = new QAction(tr("�Ƴ���"),this);
	connect(m_pRemoveStreamAction,SIGNAL(triggered()),this,SLOT(onRemoveStreamAction()));
}

void ChassisView::createContextMenu()
{
	createActions();
	m_pChassisTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pChassisTreeView,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(onCustomContextMenuRequested(const QPoint&)));
}

void ChassisView::startThread()
{
	m_pStateThread->getModuleCount(m_chassisInfo.moduleCount);
	for (int moduleIndex = 0;moduleIndex < m_chassisInfo.moduleCount;moduleIndex++)
	{
		m_pStateThread->getPortCount(moduleIndex,m_chassisInfo.portCount[moduleIndex]);
	}
	m_pStateThread->start();
}

void ChassisView::getCurrentPortPositon(S_ORDER & order)
{
	order.moduleIndex = m_currenPosion.currentModulePosion;
	order.portIndex = m_currenPosion.currentPortPosion;
}

void ChassisView::getCurrentStreamPosition(S_ORDER & order)
{
	getCurrentPortPositon(order);
	order.streamindex = m_currenPosion.currentStreamPosion;
}

void ChassisView::importFile(const QString & fileName,IMPORT_FILE_STYLE style)
{
	if (!fileName.isEmpty()) 
	{
		ImportDataDialog* importDlg = CreateImportDataDlg(style, this);
		
		if (importDlg->parseFile(fileName)) 
		{
			bool rc = importDlg->exec();
			if (rc) 
			{
				QStandardItem *portItem = getCurrentStreamIndex();

				ImportData *importData = new ImportData();
				if (importDlg->selectedFrame(*importData))
				{
					if (!addStream(m_currenPosion.currentModulePosion,m_currenPosion.currentPortPosion,importData))
					{
						return;
					}
					else
					{
						emit sendFrame(*importData);
						StreamStrategy *streamStrategy = new StreamStrategy;
						addStreamStrategy(m_currenPosion.currentModulePosion,m_currenPosion.currentPortPosion,streamStrategy);
					}
				}

				// ������Ӧ����
				ModuleData* moduleData = m_chassisData.at(m_currenPosion.currentModulePosion);
				PortData* portData = moduleData->at(m_currenPosion.currentPortPosion);
				int streamIndex = portData->size() - 1;

				if (style == IMPORT_SCD_FILE)
				{
					QStandardItem *streamItem = new QStandardItem("Stream" + QString::number(streamIndex) + 
						" " + importData->frameStyle + " " + QString::number(importData->appId,16) +
						" " + importData->iedName);
					streamItem->setData(STREAMSELECTION);
					portItem->appendRow(streamItem);
				}
				else if (style == IMPORT_PCAP_FILE)
				{
					QStandardItem *streamItem = new QStandardItem("Stream" + QString::number(streamIndex) + 
						" " + importData->frameStyle + " " + QString::number(importData->appId,16));
					streamItem->setData(STREAMSELECTION);
					portItem->appendRow(streamItem);
				}
				setSiblingDataRole(portItem,STREAMSELECTION,streamIndex);
			}
		}
		ReleaseImportDataDlg(importDlg);
	}
}

bool ChassisView::addStream(int module, int port, StreamData* streamData)
{
	bool rc = false;
	if (module < m_chassisData.size())
	{
		ModuleData* moduleData = m_chassisData.at(module);
		if (port < moduleData->size())
		{
			// ����������
			PortData* portData = moduleData->at(port);
			portData->push_back(streamData);

			// ������Ӧ����ָ��
			S_ORDER order = getCurrentOrderInfo();
			order.streamindex = portData->size() - 1;
			m_pCommunicate->sendData(PS_CREATE,order);
			if (StreamStrategySettings::getStreamEnable())
			{
				m_pCommunicate->sendData(PS_ENABLE,order);				// ʹ�ܴ�������
			}
			else
			{
				m_pCommunicate->sendData(PS_DISENABLE,order);			// ��ʹ��ʹ�ܴ�������
			}
			
			m_pCommunicate->sendData(PS_HEADERPROTOCOL,order);			// ���ñ���ͷ�Ĺ�Լ  ��̫����VLAN

			order.data = "-1";											// ������ʹ��Ĭ�ϵ�ռ��20�ֽ�
			m_pCommunicate->sendData(PS_TPLDID,order);					// ��������ID
			
			order.data = QString::number(streamData->frameLength + 4);
			m_pCommunicate->sendData(PS_PACKETLENGTH_FIXED,order);		// ���ñ��ĵĹ̶�����

			if (streamData->frameStyle.contains("GOOSE",Qt::CaseInsensitive))
			{
				order.data = QString::number(XenaManageSettings::getStreamRatePPS());
				m_pCommunicate->sendData(PS_RATEPPS_SET,order);				// ���÷������� 4000֡ÿ��
			}
			else if (streamData->frameStyle.contains("SMV",Qt::CaseInsensitive))
			{	
				order.data = QString::number(XenaManageSettings::getStreamRatePPS());
				m_pCommunicate->sendData(PS_RATEPPS_SET,order);				// ���÷������� 4000֡ÿ��
			}
			else if (streamData->frameStyle.contains("UNKNOW",Qt::CaseInsensitive))
			{
				order.data = QString::number(XenaManageSettings::getStreamRatePPS());
				m_pCommunicate->sendData(PS_RATEPPS_SET,order);				// ���÷������� 4000֡ÿ��
			}

			order.data.clear();
			for(int i = 0;i < streamData->frameLength;i++)
			{
				order.data += QString("%1").arg(streamData->frame[i], 2, 16,QChar('0'));
			}
			
			m_pCommunicate->sendData(PS_PACKETHEADER,order);			// ���ñ�������
			rc = true;
		}
	}

	return rc;
}

void ChassisView::clearChassisData()
{
	Q_FOREACH(ModuleData* moduleData, m_chassisData) 
	{
		if (moduleData)
		{
			Q_FOREACH(PortData* portData, *moduleData)
			{
				if (portData) 
				{
					Q_FOREACH(StreamData* streamData, *portData)
					{
						if(streamData) delete streamData;
					}
					delete portData;
				}
			}
		}
		delete moduleData;
	}

// 	for (int moduleCount = 0;moduleCount < m_chassisData.size();moduleCount++)
// 	{
// 		ModuleData *moduleData = m_chassisData[moduleCount];
// 		for (int portCount = 0;portCount < m_chassisData[moduleCount]->size();portCount++)
// 		{
// 			PortData *portData = m_chassisData[moduleCount]->at(portCount);
// 			for (int streamCount = 0;streamCount < portData->size();streamCount++)
// 			{
// 				StreamData *streamData = portData->at(streamCount);
// 				delete streamData;
// 			}
// 			delete portData;
// 		}
// 		delete moduleData;
// 	}
}

void ChassisView::getCurrentStreamPostion(QModelIndex & index)
{
	m_currenPosion.currentModulePosion = index.parent().parent().row();
	m_currenPosion.currentPortPosion = index.parent().row();
	m_currenPosion.currentStreamPosion = index.row();
}

StreamData* ChassisView::getCurrentStream()
{
	ModuleData* moduleData = m_chassisData.at(m_currenPosion.currentModulePosion);
	PortData* portData = moduleData->at(m_currenPosion.currentPortPosion);
	StreamData *streamData = portData->at(m_currenPosion.currentStreamPosion);
	return streamData;
}

S_ORDER ChassisView::getCurrentOrderInfo()
{
	S_ORDER order;
	order.moduleIndex = m_currenPosion.currentModulePosion;
	order.portIndex = m_currenPosion.currentPortPosion;
	order.streamindex = m_currenPosion.currentStreamPosion;
	return order;
}

QModelIndex ChassisView::getCurrentStreamModuleIndex(int moduleIndex,int portIndex,E_DATAPOSTION postion)
{
	QModelIndex chassisParentIndex = m_pChassisModel->index(0,0,QModelIndex());						// ��ȡ���ڵ㣨chassis��
	QModelIndex moduleChildIndex = m_pChassisModel->index(moduleIndex,0,chassisParentIndex);		// ��ȡ�ӽڵ㣨module��
	QModelIndex portChildIndex = m_pChassisModel->index(portIndex,postion,moduleChildIndex);		// ��ȡ���ӽڵ㣨port��
	
	return portChildIndex;
}

QStandardItem * ChassisView::getCurrentStreamIndex()
{
	QModelIndex chassisParentIndex = m_pChassisModel->index(0,0,QModelIndex());											// ��ȡ���ڵ㣨chassis��
	QModelIndex moduleChildIndex = m_pChassisModel->index(m_currenPosion.currentModulePosion,0,chassisParentIndex);		// ��ȡ�ӽڵ㣨module��
	QModelIndex portChildIndex = m_pChassisModel->index(m_currenPosion.currentPortPosion,0,moduleChildIndex);			// ��ȡ���ӽڵ㣨port��

	QStandardItem *portItem = m_pChassisModel->itemFromIndex(portChildIndex);
	return portItem;
}

void ChassisView::setSiblingDataRole(QStandardItem *standardItem,E_SELECTION_TYPE role,int index)
{
	for (int i = SYNCPOSTION;i <= OWNERPOSITION;i++)
	{
		QStandardItem *itemData = new QStandardItem();
		itemData->setData(role);
		standardItem->setChild(index,i,itemData);
	}
}

void ChassisView::addStreamStrategy(int module, int port, StreamStrategy* streamStrategy)
{
	if (module < m_chassisStrategy.size())
	{
		ModuleStrategy* moduleStategy = m_chassisStrategy.at(module);
		if (port < moduleStategy->size())
		{
			// ����������ӵ��˿ڲ�����
			PortStrategy* portStategy = moduleStategy->at(port);
			portStategy->streamStrategy.push_back(streamStrategy);

			emit updatePortStrategy(portStategy);
		}
	}
}

PortStrategy * ChassisView::getCurrentPortStrategy()
{
	ModuleStrategy* moduleStrategy = m_chassisStrategy.at(m_currenPosion.currentModulePosion);
	PortStrategy* portStrategy = moduleStrategy->at(m_currenPosion.currentPortPosion);
	return portStrategy;
}

StreamStrategy * ChassisView::getCurrentStreamStrategy()
{
	PortStrategy * portStrategy = getCurrentPortStrategy();
	StreamStrategy *streamStrategy = portStrategy->streamStrategy.at(m_currenPosion.currentStreamPosion);
	return streamStrategy;
}

void ChassisView::clearChassisStrategy()
{
	Q_FOREACH(ModuleStrategy* moduleStrategy, m_chassisStrategy) 
	{
		if (moduleStrategy)
		{
			Q_FOREACH(PortStrategy* portStrategy, *moduleStrategy)
			{
				if (portStrategy) 
				{
					Q_FOREACH(StreamStrategy* streamStrategy, portStrategy->streamStrategy)
					{
						if(streamStrategy) delete streamStrategy;
					}
					delete portStrategy;
				}
			}
		}
		delete moduleStrategy;
	}
}

void ChassisView::setTreeViewData()
{
	QStandardItem *chassisItem = new QStandardItem(m_chassisInfo.chassisName);
	chassisItem->setData(CHASSISSELECTION);
	m_pChassisModel->appendRow(chassisItem);

	for (int i = SYNCPOSTION;i <= OWNERPOSITION;i++)
	{
		QStandardItem *chassisItemData = new QStandardItem();
		chassisItemData->setData(CHASSISSELECTION);
		m_pChassisModel->setItem(0,i,chassisItemData);
	}

	S_ORDER order;
	m_pCommunicate->sendData(C_RESERVEDBY,order);						// ��ѯ����ռ�����

	//  ��ʼ��������
	m_chassisData.clear();
	for(int moduleCount = 0; moduleCount < m_chassisInfo.moduleCount;moduleCount++) 
	{
		ModuleData* moduleData = new ModuleData();
		for (int portCount = 0;portCount < m_chassisInfo.portCount[moduleCount];portCount++) 
		{
			PortData *portData = new PortData();
			moduleData->push_back(portData);
		}
		m_chassisData.push_back(moduleData);
	}

	// ��ʼ��������Ϣ
	m_chassisStrategy.clear();
	for(int moduleCount = 0; moduleCount < m_chassisInfo.moduleCount;moduleCount++) 
	{
		ModuleStrategy* moduleStrategy = new ModuleStrategy();
		for (int portCount = 0;portCount < m_chassisInfo.portCount[moduleCount];portCount++) 
		{
			PortStrategy *portStrategy = new PortStrategy();
			moduleStrategy->push_back(portStrategy);
		}
		m_chassisStrategy.push_back(moduleStrategy);
	}
	
	//  ��ʼ��������Ϣ
	for (int moduleCount = 0;moduleCount < m_chassisInfo.moduleCount;moduleCount++)
	{
		S_ORDER moduleOrder;
		moduleOrder.moduleIndex = moduleCount;

		QStandardItem *moduleItem = new QStandardItem(m_chassisInfo.moduleName[moduleCount]);
		moduleItem->setData(MODULESELECTION);
		chassisItem->appendRow(moduleItem);
		setSiblingDataRole(chassisItem,MODULESELECTION,moduleCount);

		m_pCommunicate->sendData(M_RESERVEDBY,moduleOrder);				// ��ѯ�忨ռ��״̬

		for (int portCount = 0;portCount < m_chassisInfo.portCount[moduleCount];portCount++)
		{
			S_ORDER portOrder;
			portOrder.moduleIndex = moduleCount;
			portOrder.portIndex = portCount;
			
			QStandardItem *portItem = new QStandardItem(m_chassisInfo.portName[moduleCount][portCount]);
			portItem->setData(PORTSELECTION);
			moduleItem->appendRow(portItem);
			setSiblingDataRole(moduleItem,PORTSELECTION,portCount);

			m_pCommunicate->sendData(P_RESERVEDBY,portOrder);			// ��ѯ�˿�ռ��״̬
			m_pCommunicate->sendData(P_TRAFFIC_QUERY,portOrder);		// ��ѯ�˿��Ƿ����ڷ�������
			m_pCommunicate->sendData(P_RECEIVESYNC,portOrder);			// ��ѯ�˿ڵ�ͬ��״̬
		}
	}
	m_pChassisTreeView->expandAll();

	// ������ѯ״̬�߳�
	startThread();
}
