#include "ChassisViewWidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/chassis.h"
#include "XenaManager/src/Communicate/deviceImpl.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include "XenaManager/src/Communicate/stream.h"
#include <QAction>
#include <QMenu>
#include <QTimer>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QBrush>
#include <QMessageBox>
#include <QDebug>

#define MAXWIDGETWIDTH 520

ChassisViewWidget::ChassisViewWidget(CXenaTester *xenaTester,QWidget *parent)
    : QWidget(parent),
    m_pXenaTester(xenaTester)
{
    m_pDevice = m_pXenaTester->device();

    initUI();

    connect(m_pUpdateStatusTimer,SIGNAL(timeout()),this,SLOT(updateChassisStatus()));
    connect(m_pChassisTreeView,SIGNAL(clicked(QModelIndex)),this,SLOT(onViewClicked(QModelIndex)));
}

ChassisViewWidget::~ChassisViewWidget()
{

}

void ChassisViewWidget::initTreeViewWidgetData()
{
    m_pChassis = m_pXenaTester->chassis();

    if (!m_pChassis)
    {
        return;
    }
    qDebug() << m_pChassis->chassisName();

    // 增加机箱信息
    QStandardItem *chassisItem = new QStandardItem(m_pChassis->chassisName());
    chassisItem->setData(CHASSISSELECTION);
    m_pChassisModel->appendRow(chassisItem);

    for (int i = SYNCINDEX;i <= OWNERINDEX;i++)
    {
        QStandardItem *chassisItemData = new QStandardItem();
        chassisItemData->setData(CHASSISSELECTION);
        m_pChassisModel->setItem(0,i,chassisItemData);
    }

    // 增加板卡信息
    for (int moduleIndex = 0;moduleIndex < m_pChassis->moduleCount();moduleIndex++)
    {
        QStandardItem *moduleItem = new QStandardItem(m_pChassis->module(moduleIndex)->name());
        moduleItem->setData(MODULESELECTION);
        chassisItem->appendRow(moduleItem);
        setSiblingDataRole(chassisItem,MODULESELECTION,moduleIndex);

        // 增加端口信息
        for (int portIndex = 0;portIndex < m_pChassis->module(moduleIndex)->portCount();portIndex++)
        {
            QStandardItem *portItem = new QStandardItem(m_pChassis->module(moduleIndex)->port(portIndex)->portName());
            portItem->setData(PORTSELECTION);
            moduleItem->appendRow(portItem);
            setSiblingDataRole(moduleItem,PORTSELECTION,portIndex);
        }
    }
    m_pChassisTreeView->expandAll();

    updateChassisStatus();
    startUpdate();                                                      // 开始更新状态
}

void ChassisViewWidget::onReserveAction()
{
    switch(m_currenIndex.currentType)
    {
    case CHASSISSELECTION:
        {
            m_pDevice->reserveChassis();
            updateChassisOwner();
            break;
        }
    case MODULESELECTION:
        {
            m_pDevice->reserveModule(m_currenIndex.currentModuleIndex);
            updateModuleOwner(m_currenIndex.currentModuleIndex);
            break;
        }
    case PORTSELECTION:
        {
            m_pDevice->reservePort(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);
            updatePortOwner(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);

            emit updateConfigWidget(m_currenIndex);                     // 占用后再次进行更新端口信息
            break;
        }
    default:
        break;
    }
}

void ChassisViewWidget::onReleaseAction()
{
	switch(m_currenIndex.currentType)
	{
	case CHASSISSELECTION:
		{
            m_pDevice->releaseChassis();
            updateChassisOwner();
			break;
		}
	case MODULESELECTION:
		{
            m_pDevice->releaseModule(m_currenIndex.currentModuleIndex);
            updateModuleOwner(m_currenIndex.currentModuleIndex);
			break;
		}
	case PORTSELECTION:
		{
            m_pDevice->releasePort(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);
            updatePortOwner(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);

            emit updateConfigWidget(m_currenIndex);                     // 释放后再次进行更新端口信息
			break;
		}
	default:
		break;
	}
}

void ChassisViewWidget::onRelinquishAction()
{
	switch(m_currenIndex.currentType)
	{
	case CHASSISSELECTION:
		{
            m_pDevice->relinquishChassis();
            updateChassisOwner();
			break;
		}
	case MODULESELECTION:
		{
            m_pDevice->relinquishModule(m_currenIndex.currentModuleIndex);
            updateModuleOwner(m_currenIndex.currentModuleIndex);
			break;
		}
	case PORTSELECTION:
		{
            m_pDevice->relinquishPort(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);
            updatePortOwner(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);

            emit updateConfigWidget(m_currenIndex);                     // 注销后再次进行更新端口信息
			break;
		}
	default:
		break;
	}
}

void ChassisViewWidget::onImportScdAction()
{
    QString scdFileName = QFileDialog::getOpenFileName(0,tr("打开SCD文件"),QString(),
        tr("SCL (*.scd *.icd *.cid);; 所有文件(*.*)"));

    importFile(scdFileName,IMPORT_SCD_FILE);
}

void ChassisViewWidget::onImportPcapAction()
{
    QString pcapFileName = QFileDialog::getOpenFileName(0,tr("打开PCAP文件"),QString(),
        tr("PCAP (*.pcap);; 所有文件(*.*)"));

    importFile(pcapFileName,IMPORT_PCAP_FILE);
}

void ChassisViewWidget::onRemoveStreamAction()
{
    CStream *stream = m_pChassis->module(m_currenIndex.currentModuleIndex)->
        port(m_currenIndex.currentPortIndex)->stream(m_currenIndex.currentStreamIndex);
    
    // 对应的删除指令
    m_pDevice->delStream(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,stream);
    
    // 对应的CStream的内容的删除
    m_pChassis->module(m_currenIndex.currentModuleIndex)->port(m_currenIndex.currentPortIndex)->removeStream(stream);
    delete stream;
    // 删除树形控件中的内容
    QStandardItem *portItem = getCurrentPortItem();
    portItem->removeRow(m_currenIndex.currentStreamIndex);

    if (0 == m_pChassis->module(m_currenIndex.currentModuleIndex)->port(m_currenIndex.currentPortIndex)->streamCount())
    {
        m_currenIndex.currentType = PORTSELECTION;

        emit updateConfigWidget(m_currenIndex);
    }
}

void ChassisViewWidget::onCustomContextMenuRequested(const QPoint &point)
{
	QModelIndex index = m_pChassisTreeView->indexAt(point);
	QVariant var = index.data(Qt::UserRole + 1);
	if (!var.isValid())
	{
		return;
	}

	QMenu contextMenu;
	m_currenIndex.currentType =  static_cast<E_SELECTION_TYPE>(var.toInt());

	switch(var.toInt())
	{
	case CHASSISSELECTION:
        {
            QString chassisOwner;
            m_pDevice->chassisReservedOwner(chassisOwner);

            if (chassisOwner.isEmpty())
            {
                contextMenu.addAction(m_pReserveAction);
            }
            else
            {
                if (QString(LoginSettings::getUserName().data()) != chassisOwner)
                {
                    contextMenu.addAction(m_pRelinquishAction);
                }
                else
                {
                    contextMenu.addAction(m_pReleaseAction);
                    contextMenu.addAction(m_pRelinquishAction);
                }
            }
            contextMenu.exec(QCursor::pos());  
            break;
        }
	case MODULESELECTION:
        {
            m_currenIndex.currentModuleIndex = index.row();

            QString moduleOwner;
            m_pDevice->moduleReservedOwner(m_currenIndex.currentModuleIndex,moduleOwner);

            if (moduleOwner.isEmpty())
            {
                contextMenu.addAction(m_pReserveAction);
            }
            else
            {
                if (QString(LoginSettings::getUserName().data()) != moduleOwner)
                {
                    contextMenu.addAction(m_pRelinquishAction);
                }
                else
                {
                    contextMenu.addAction(m_pReleaseAction);
                    contextMenu.addAction(m_pRelinquishAction);
                }
            }
            contextMenu.exec(QCursor::pos());  
            break;
        }
	case PORTSELECTION:
        {
            m_currenIndex.currentModuleIndex = index.parent().row();
            m_currenIndex.currentPortIndex = index.row();

            QString portOwner;
            m_pDevice->portReservedOwner(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,portOwner);

            if (portOwner.isEmpty())
            {
                contextMenu.addAction(m_pReserveAction);
            }
            else
            {
                if (QString(LoginSettings::getUserName().data()) != portOwner)
                {
                    contextMenu.addAction(m_pRelinquishAction);
                }
                else
                {
                    contextMenu.addAction(m_pReleaseAction);
                    contextMenu.addAction(m_pRelinquishAction);
                    contextMenu.addSeparator();
                    contextMenu.addAction(m_pImportScdAction);
                    contextMenu.addAction(m_pImportPcapAction);

                    if (m_pChassis->port(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex)->streamCount() <
                        m_pChassis->port(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex)->maxStreamNum())
                    {
                        m_pImportScdAction->setEnabled(true);
                        m_pImportPcapAction->setEnabled(true);
                    }
                    else
                    {
                        m_pImportScdAction->setEnabled(false);
                        m_pImportPcapAction->setEnabled(false);
                    }
                }
            }
            contextMenu.exec(QCursor::pos());  
            break;
        }
	case STREAMSELECTION:
		{
			getCurrentStreamIndex(index);

            QString portOwner;
            m_pDevice->portReservedOwner(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,portOwner);

            if (QString(LoginSettings::getUserName().data()) == portOwner)
            {
                contextMenu.addAction(m_pRemoveStreamAction);
                contextMenu.exec(QCursor::pos());  
            }
			break;
		}
	default:
		break;
	}  
}

void ChassisViewWidget::onViewClicked(QModelIndex index)
{
	if (index.column() > 0)
	{
		index = index.sibling(index.row(),0);
	}

	QStandardItem *item = m_pChassisModel->itemFromIndex(index);

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
		m_currenIndex.currentModuleIndex = index.row();
		break;
	case PORTSELECTION:
		{
			m_currenIndex.currentModuleIndex = index.parent().row();
			m_currenIndex.currentPortIndex = index.row();
			break;
		}
	case STREAMSELECTION:
		{
 			getCurrentStreamIndex(index);

			break;
		}
	default:
		break;
	}
	m_currenIndex.currentType =  static_cast<E_SELECTION_TYPE>(var.toInt());

	emit updateConfigWidget(m_currenIndex);
}

void ChassisViewWidget::updateChassisStatus()
{
    m_pDevice->keepAlive();                               // 保持在线

    // 实时更新机箱的占用者
    updateChassisOwner();

    // 实时更新板卡的占用者
    for (int moduleIndex = 0;moduleIndex < m_pChassis->moduleCount();moduleIndex++)
    {
        updateModuleOwner(moduleIndex);

        // 实时更新端口的名称、同步状态、发送状态、占用者
        for (int portIndex = 0;portIndex < m_pChassis->module(moduleIndex)->portCount();portIndex++)
        {
            updatePortName(moduleIndex,portIndex);

            updatePortCommunicationStatus(moduleIndex,portIndex);

            updatePortOwner(moduleIndex,portIndex);
        }
    }
}

void ChassisViewWidget::initUI()
{
    m_pUpdateStatusTimer = new QTimer(this);


    m_pChassisTreeView = new QTreeView;
    m_pChassisTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pChassisTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pChassisTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_pChassisModel = new QStandardItemModel(this);
    m_pChassisTreeView->setModel(m_pChassisModel);
    QStringList chassisStringList;
    chassisStringList<< tr("名称") << tr("同步") << tr("状态") << tr("占用者");
    m_pChassisModel->setHorizontalHeaderLabels(chassisStringList);

    createContextMenu();
    setMainLayout();
}

void ChassisViewWidget::setMainLayout()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pChassisTreeView);
    mainLayout->setMargin(1);

    this->setLayout(mainLayout);
    this->setMaximumWidth(MAXWIDGETWIDTH);
}

void ChassisViewWidget::createActions()
{
    m_pReserveAction = new QAction(tr("占用"),this);
    m_pReserveAction->setIcon(QIcon(":/images/icon_occupy.png"));
    connect(m_pReserveAction,SIGNAL(triggered()),this,SLOT(onReserveAction()));

    m_pReleaseAction = new QAction(tr("释放"),this);
    m_pReleaseAction->setIcon(QIcon(":/images/icon_release.png"));
    connect(m_pReleaseAction,SIGNAL(triggered()),this,SLOT(onReleaseAction()));

    m_pRelinquishAction = new QAction(tr("注销"),this);
    m_pRelinquishAction->setIcon(QIcon(":/images/icon_Reset.png"));
    connect(m_pRelinquishAction,SIGNAL(triggered()),this,SLOT(onRelinquishAction()));

    m_pImportScdAction = new QAction(tr("导入流(SCD)"),this);
    m_pImportScdAction->setIcon(QIcon(":/images/icon_Import_SCD.png"));
    connect(m_pImportScdAction,SIGNAL(triggered()),this,SLOT(onImportScdAction()));

    m_pImportPcapAction = new QAction(tr("导入流(PCAP)"),this);
    m_pImportPcapAction->setIcon(QIcon(":/images/icon_Import_PCAP.png"));
    connect(m_pImportPcapAction,SIGNAL(triggered()),this,SLOT(onImportPcapAction()));

    m_pRemoveStreamAction = new QAction(tr("移除流"),this);
    connect(m_pRemoveStreamAction,SIGNAL(triggered()),this,SLOT(onRemoveStreamAction()));
}

void ChassisViewWidget::createContextMenu()
{
    createActions();
    m_pChassisTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_pChassisTreeView,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(onCustomContextMenuRequested(const QPoint&)));
}

void ChassisViewWidget::startUpdate()
{
    m_pUpdateStatusTimer->start(XenaManageSettings::stateUpdateTime());
}

void ChassisViewWidget::importFile(const QString & fileName,IMPORT_FILE_STYLE style)
{
    if (!fileName.isEmpty()) 
    {
        ImportDataDialog* importDlg = CreateImportDataDlg(style, this);

        if (importDlg->parseFile(fileName)) 
        {
            bool rc = importDlg->exec();
            if (rc) 
            {
                QStandardItem *portItem = getCurrentPortItem();

                ImportData importData;
                if (importDlg->selectedFrame(importData))
                {
                    if (importData.frameLength > m_pChassis->module(m_currenIndex.currentModuleIndex)
                        ->port(m_currenIndex.currentPortIndex)->maxHeaderLength())
                    {
                        QMessageBox::warning(0,tr("警告"),tr("报文长度大于端口允许最大长度，请重新设置端口的帧头长度！"),QString(tr("确定")));
                        ReleaseImportDataDlg(importDlg);
                        return;
                    }
                    else
                    {
                        if (m_pXenaTester->addStream(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,&importData))
                        {
                            ;
                        }
                        else
                        {
                            ReleaseImportDataDlg(importDlg);
                            return;
                        }
                    }
                }

                int streamIndex = m_pChassis->module(m_currenIndex.currentModuleIndex)->
                    port(m_currenIndex.currentPortIndex)->streamCount() - 1;

                QString streamName = m_pChassis->module(m_currenIndex.currentModuleIndex)->
                    port(m_currenIndex.currentPortIndex)->stream(streamIndex)->streamName();
                // 在树形控件中增加流内容
                QStandardItem *streamItem = new QStandardItem(streamName);
                streamItem->setData(STREAMSELECTION);
                portItem->appendRow(streamItem);
                setSiblingDataRole(portItem,STREAMSELECTION,streamIndex);
            }
        }
        ReleaseImportDataDlg(importDlg);
    }
}

void ChassisViewWidget::setSiblingDataRole(QStandardItem *standardItem,E_SELECTION_TYPE role,int index)
{
	for (int i = SYNCINDEX;i <= OWNERINDEX;i++)
	{
		QStandardItem *itemData = new QStandardItem();
		itemData->setData(role);
		standardItem->setChild(index,i,itemData);
	}
}

void ChassisViewWidget::updateChassisOwner()
{
    QString chassisOwner;
    m_pDevice->chassisReservedOwner(chassisOwner);

    QModelIndex chassisOwnerIndex = m_pChassisModel->index(0,OWNERINDEX,QModelIndex());
    m_pChassisModel->setData(chassisOwnerIndex,chassisOwner);
}

void ChassisViewWidget::updateModuleOwner(int moduleIndex)
{
    QModelIndex chassisIndex = m_pChassisModel->index(0,0,QModelIndex());

    QString moduleOwner;
    QModelIndex moduleOwnerModelIndex = m_pChassisModel->index(moduleIndex,OWNERINDEX,chassisIndex);
    m_pDevice->moduleReservedOwner(moduleIndex,moduleOwner);
    m_pChassisModel->setData(moduleOwnerModelIndex,moduleOwner);
}

void ChassisViewWidget::updatePortName(int moduleIndex,int portIndex)
{
    QModelIndex chassisIndex = m_pChassisModel->index(0,0,QModelIndex());
    QModelIndex moduleModelIndex = m_pChassisModel->index(moduleIndex,0,chassisIndex);
    QModelIndex portNameModelIndex = m_pChassisModel->index(portIndex,NAMEINDEX,moduleModelIndex);

    QString oldPortName = m_pChassisModel->data(portNameModelIndex).toString();

    QString portName;
    m_pDevice->portName(moduleIndex,portIndex,portName);

    if (oldPortName == portName)
    {
        return;
    }

    m_pChassisModel->setData(portNameModelIndex,portName);
}

void ChassisViewWidget::updatePortCommunicationStatus(int moduleIndex,int portIndex)
{
    QModelIndex chassisIndex = m_pChassisModel->index(0,0,QModelIndex());
    QModelIndex moduleModelIndex = m_pChassisModel->index(moduleIndex,0,chassisIndex);

    bool linked;
    bool trafficed;
    QString syncStatus,trafficStatus;
    QModelIndex portSyncModelIndex = m_pChassisModel->index(portIndex,SYNCINDEX,moduleModelIndex);
    m_pDevice->portCommunicationStatus(moduleIndex,portIndex,linked,trafficed);
    if (linked)
    {
        syncStatus = "YES";
    }
    else
    {
        syncStatus = "NO";
    }
    m_pChassisModel->setData(portSyncModelIndex,syncStatus);


    if (trafficed)
    {
        trafficStatus = "ON";
    }
    else
    {
        trafficStatus = "OFF";
    }
    QModelIndex portTrafficModelIndex = m_pChassisModel->index(portIndex,STATUSINDEX,moduleModelIndex);
    m_pChassisModel->setData(portTrafficModelIndex,trafficStatus);

    emit updatePortTraffic(moduleIndex,portIndex);
}

void ChassisViewWidget::updatePortOwner(int moduleIndex,int portIndex)
{
    QModelIndex chassisIndex = m_pChassisModel->index(0,0,QModelIndex());
    QModelIndex moduleModelIndex = m_pChassisModel->index(moduleIndex,0,chassisIndex);

    QString portOwner;
    QModelIndex portOwnerModelIndex = m_pChassisModel->index(portIndex,OWNERINDEX,moduleModelIndex);
    m_pDevice->portReservedOwner(moduleIndex,portIndex,portOwner);
    m_pChassisModel->setData(portOwnerModelIndex,portOwner);

    QStandardItem *portNameItem = m_pChassisModel->itemFromIndex(portOwnerModelIndex.sibling(portOwnerModelIndex.row(),0));
    
    m_pChassis->module(moduleIndex)->port(portIndex)->setReserved(true); // 设置对应的port的占用状态
    m_pChassis->module(moduleIndex)->port(portIndex)->setReservedBy(portOwner);

    if (portOwner.isEmpty())
    {
        QBrush brush(QColor("black"));
        portNameItem->setForeground(brush);
    }
    else
    {
        if (QString(LoginSettings::getUserName().data()) != portOwner)
        {
            QBrush brush(QColor("gray"));
            portNameItem->setForeground(brush);
        }
        else
        {
            QBrush brush(QColor("green"));
            portNameItem->setForeground(brush);;
        }
    }
}

QStandardItem* ChassisViewWidget::getCurrentPortItem()
{
    QModelIndex chassisParentIndex = m_pChassisModel->index(0,0,QModelIndex());											// 获取父节点（chassis）
    QModelIndex moduleChildIndex = m_pChassisModel->index(m_currenIndex.currentModuleIndex,0,chassisParentIndex);		// 获取子节点（module）
    QModelIndex portChildIndex = m_pChassisModel->index(m_currenIndex.currentPortIndex,0,moduleChildIndex);			// 获取孙子节点（port）

    QStandardItem *portItem = m_pChassisModel->itemFromIndex(portChildIndex);
    return portItem;
}

QModelIndex ChassisViewWidget::getCurrentPortModelIndex(int moduleIndex,int portIndex)
{
    QModelIndex chassisIndex = m_pChassisModel->index(0,0,QModelIndex());
    QModelIndex moduleModelIndex = m_pChassisModel->index(moduleIndex,0,chassisIndex);
    QModelIndex portModelIndex = m_pChassisModel->index(portIndex,0,moduleModelIndex);

    return portModelIndex;
}

void ChassisViewWidget::getCurrentStreamIndex(QModelIndex & index)
{
    m_currenIndex.currentModuleIndex = index.parent().parent().row();
    m_currenIndex.currentPortIndex = index.parent().row();
    m_currenIndex.currentStreamIndex = index.row();
}

