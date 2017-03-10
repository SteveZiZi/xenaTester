#include "ChassisViewWidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/chassis.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include "XenaManager/src/Communicate/stream.h"
#include "XenaManager/src/inc/xt_utility.h"
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

#define MAX_WIDTH 520
#define MIN_WIDTH      200

ChassisViewWidget::ChassisViewWidget(CXenaTester *xenaTester,QWidget *parent)
    : QWidget(parent),
    m_pXenaTester(xenaTester)
{

    initUI();

    connect(m_pKeepAliveTimer,SIGNAL(timeout()),this,SLOT(onKeepAliveTimerOut()));
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
            CPort *port = m_pChassis->module(moduleIndex)->port(portIndex);
            QStandardItem *portItem = new QStandardItem(port->portName());
            portItem->setData(PORTSELECTION);
            moduleItem->appendRow(portItem);
            setSiblingDataRole(moduleItem,PORTSELECTION,portIndex);
            // 增加流信息
            for (int streamIndex = 0;streamIndex < port->streamCount();streamIndex++)
            {
                QStandardItem *streamItem = new QStandardItem(port->stream(streamIndex)->streamName());
                streamItem->setData(STREAMSELECTION);
                portItem->appendRow(streamItem);
                setSiblingDataRole(portItem,STREAMSELECTION,streamIndex);
            }
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
            m_pXenaTester->reserveChassis();
            updateChassisOwner();
            break;
        }
    case MODULESELECTION:
        {
            m_pXenaTester->reserveModule(m_currenIndex.currentModuleIndex);
            updateModuleOwner(m_currenIndex.currentModuleIndex);
            break;
        }
    case PORTSELECTION:
        {
            m_pXenaTester->reservePort(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);
            getPortConfigInfo(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);

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
            m_pXenaTester->releaseChassis();
            updateChassisOwner();
			break;
		}
	case MODULESELECTION:
		{
            m_pXenaTester->releaseModule(m_currenIndex.currentModuleIndex);
            updateModuleOwner(m_currenIndex.currentModuleIndex);
			break;
		}
	case PORTSELECTION:
		{
            m_pXenaTester->releasePort(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);
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
            m_pXenaTester->relinquishChassis();
            updateChassisOwner();
			break;
		}
	case MODULESELECTION:
		{
            m_pXenaTester->relinquishModule(m_currenIndex.currentModuleIndex);
            updateModuleOwner(m_currenIndex.currentModuleIndex);
			break;
		}
	case PORTSELECTION:
		{
            m_pXenaTester->relinquishPort(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex);
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
    QString scdFileName = QFileDialog::getOpenFileName(0,tr("Open SCD File"),QString(),
                                                       tr("SCL (*.scd *.icd *.cid);; All Files(*.*)"));

    importFile(scdFileName,IMPORT_SCD_FILE);
}

void ChassisViewWidget::onImportPcapAction()
{
    QString pcapFileName = QFileDialog::getOpenFileName(0,tr("Open PCAP File"),QString(),
                                                        tr("PCAP (*.pcap);; All Files(*.*)"));

    importFile(pcapFileName,IMPORT_PCAP_FILE);
}

void ChassisViewWidget::onRemoveStreamAction()
{
    int mIndex = m_currenIndex.currentModuleIndex;
    int pIndex = m_currenIndex.currentPortIndex;
    int sIndex = m_currenIndex.currentStreamIndex;

    delStream(mIndex,pIndex,sIndex);

    if (0 == m_pChassis->port(mIndex, pIndex)->streamCount())
    {
        m_currenIndex.currentType = PORTSELECTION;
        emit updateConfigWidget(m_currenIndex);
    }
}

void ChassisViewWidget::onImportAction()
{
    // Import
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Import Test Template"),".",
        tr("Test Template(*.xml)"));
    if(!fileName.isEmpty())
    {
        switch(m_currenIndex.currentType)
        {
        case CHASSISSELECTION:
            {
                break;
            }
        case MODULESELECTION:
            {
                onImportModuleConf(m_currenIndex.currentModuleIndex,fileName);

                emit updateConfigWidget(m_currenIndex);

                break;
            }
        case PORTSELECTION:
            {
                onImportPortConf(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,fileName);

                emit updateConfigWidget(m_currenIndex);

                break;
            }
        default:
            break;
        }
    }
}

void ChassisViewWidget::onExportAction()
{
    switch(m_currenIndex.currentType)
    {
    case CHASSISSELECTION:
        {
            break;
        }
    case MODULESELECTION:
        {
            QString fileName = QFileDialog::getSaveFileName(this,
                tr("Export Test Template"),
                "module",
                tr("Test Template(*.xml)"));
            if (!fileName.isEmpty())
            {
                m_pXenaTester->exportModule(m_currenIndex.currentModuleIndex,fileName);
            }
            break;
        }
    case PORTSELECTION:
        {
            QString fileName = QFileDialog::getSaveFileName(this,
                tr("Export Test Template"),
                "port",
                tr("Test Template(*.xml)"));
            if (!fileName.isEmpty())
            {
                m_pXenaTester->exportPort(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,fileName);
            }
            break;
        }
    default:
        break;
    }

    // Export
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
            m_pXenaTester->chassisReservedOwner(chassisOwner);

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
            m_pXenaTester->moduleReservedOwner(m_currenIndex.currentModuleIndex,moduleOwner);

            if (!m_pChassis->module(m_currenIndex.currentModuleIndex)->isBadModule())
            {
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
                        contextMenu.addSeparator();
                        m_pImportAction->setText(tr("Import module config"));
                        m_pExportAction->setText(tr("Export module config"));
                        contextMenu.addAction(m_pImportAction);
                        contextMenu.addAction(m_pExportAction);
                    }
                }
                contextMenu.exec(QCursor::pos());  
            }
            break;
        }
	case PORTSELECTION:
        {
            m_currenIndex.currentModuleIndex = index.parent().row();
            m_currenIndex.currentPortIndex = index.row();

            QString portOwner;
            m_pXenaTester->portReservedOwner(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,portOwner);

            if (!m_pChassis->module(m_currenIndex.currentModuleIndex)->port(m_currenIndex.currentPortIndex)->isBadPort())
            {
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
                        contextMenu.addSeparator();
                        m_pImportAction->setText(tr("Import port config"));
                        m_pExportAction->setText(tr("Export port config"));
                        contextMenu.addAction(m_pImportAction);
                        contextMenu.addAction(m_pExportAction);


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
            }
            break;
        }
	case STREAMSELECTION:
		{
			getCurrentStreamIndex(index);

            QString portOwner;
            m_pXenaTester->portReservedOwner(m_currenIndex.currentModuleIndex,m_currenIndex.currentPortIndex,portOwner);

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

void ChassisViewWidget::onKeepAliveTimerOut()
{
    m_pXenaTester->keepAlive();                               // 保持在线
}

void ChassisViewWidget::initUI()
{
    m_pUpdateStatusTimer = new QTimer(this);
    m_pKeepAliveTimer = new QTimer(this);

    m_pChassisTreeView = new QTreeView;
    m_pChassisTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pChassisTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pChassisTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_pChassisModel = new QStandardItemModel(this);
    m_pChassisTreeView->setModel(m_pChassisModel);
    QStringList chassisStringList;
    chassisStringList<< tr("Name") << tr("Sync") << tr("Status") << tr("Owner");
    m_pChassisModel->setHorizontalHeaderLabels(chassisStringList);

    createContextMenu();
    setMainLayout();
    this->setMinimumWidth(MIN_WIDTH);
    this->setMaximumWidth(MAX_WIDTH);
}

void ChassisViewWidget::setMainLayout()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pChassisTreeView);
    mainLayout->setMargin(1);

    this->setLayout(mainLayout);
}

void ChassisViewWidget::createActions()
{
    m_pReserveAction = new QAction(tr("Reserve"),this);
    m_pReserveAction->setIcon(QIcon(":/images/icon_occupy.png"));
    connect(m_pReserveAction,SIGNAL(triggered()),this,SLOT(onReserveAction()));

    m_pReleaseAction = new QAction(tr("Release"),this);
    m_pReleaseAction->setIcon(QIcon(":/images/icon_release.png"));
    connect(m_pReleaseAction,SIGNAL(triggered()),this,SLOT(onReleaseAction()));

    m_pRelinquishAction = new QAction(tr("Relinquish"),this);
    m_pRelinquishAction->setIcon(QIcon(":/images/icon_Reset.png"));
    connect(m_pRelinquishAction,SIGNAL(triggered()),this,SLOT(onRelinquishAction()));

    m_pImportScdAction = new QAction(tr("Import Stream(SCD)"),this);
    m_pImportScdAction->setIcon(QIcon(":/images/icon_Import_SCD.png"));
    connect(m_pImportScdAction,SIGNAL(triggered()),this,SLOT(onImportScdAction()));

    m_pImportPcapAction = new QAction(tr("Import Stream(PCAP)"),this);
    m_pImportPcapAction->setIcon(QIcon(":/images/icon_Import_PCAP.png"));
    connect(m_pImportPcapAction,SIGNAL(triggered()),this,SLOT(onImportPcapAction()));

    m_pRemoveStreamAction = new QAction(tr("Del Stream"),this);
    connect(m_pRemoveStreamAction,SIGNAL(triggered()),this,SLOT(onRemoveStreamAction()));

    m_pImportAction = new QAction(tr("Import"),this);
    m_pImportAction->setIcon(QIcon(":/images/import.png"));
    connect(m_pImportAction,SIGNAL(triggered()),this,SLOT(onImportAction()));

    m_pExportAction = new QAction(tr("Export"),this);
    m_pExportAction->setIcon(QIcon(":/images/export.png"));
    connect(m_pExportAction,SIGNAL(triggered()),this,SLOT(onExportAction()));
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
    m_pKeepAliveTimer->start(XenaManageSettings::breathTime());
}

void ChassisViewWidget::importFile(const QString & fileName,IMPORT_FILE_STYLE style)
{
    QStandardItem *portItem = getCurrentPortItem();
    int moduleIndex = m_currenIndex.currentModuleIndex;
    int portIndex = m_currenIndex.currentPortIndex;
    CPort * port = m_pChassis->port(moduleIndex, portIndex);

    if(port->streamCount() >= port->maxStreamNum()) {
        QMessageBox::information(this, tr("Error"), tr("this port has reached max number of stream"));
        return ;
    }

    if (!fileName.isEmpty()) 
    {
        ImportDataDialog* importDlg = CreateImportDataDlg(style, this);

        if (importDlg->parseFile(fileName)) 
        {
            bool rc = importDlg->exec();
            if (rc) 
            {
#if 0
                ImportData importData;
                if (importDlg->selectedFrame(importData)) {
                    if (importData.frameLength > 1024) {
                        QMessageBox::warning(this, tr("Error"), tr("frame length must less than 1024"));
                        ReleaseImportDataDlg(importDlg);
                        return;
                    }
                    if (importData.frameLength > port->maxHeaderLength()) {
                        m_pXenaTester->setPortMaxHeaderLength(moduleIndex, portIndex, 1024);
                    }
                    if (!m_pXenaTester->addStream(moduleIndex, portIndex, &importData)) {
                        ReleaseImportDataDlg(importDlg);
                        return;
                    }

                    /*
                     *  在树形控件中增加流内容
                     */
                    int streamIndex = port->streamCount() - 1;
                    QString streamName = port->stream(streamIndex)->streamName();
                    QStandardItem *streamItem = new QStandardItem(streamName);
                    streamItem->setData(STREAMSELECTION);
                    portItem->appendRow(streamItem);
                    setSiblingDataRole(portItem,STREAMSELECTION,streamIndex);
                }
#else 
                QList<ImportData*> importDataList;
                if (importDlg->selectedFrame(importDataList))
                {
                    if(port->streamCount() + importDataList.size() > port->maxStreamNum()) {
                        QMessageBox::information(this, tr("Error"), tr("this port has reached max number of stream"));
                        return ;
                    }

                    for (int i = 0;i < importDataList.size();i++)
                    {
                        if (importDataList[i]->frameLength > 1024) {
                            QMessageBox::warning(this, tr("Error"), tr("frame %1's length large than 1024").arg(i + 1));
                            ReleaseImportDataDlg(importDlg);
                            return;
                        }
                        if (importDataList[i]->frameLength > port->maxHeaderLength()) {
                            m_pXenaTester->setPortMaxHeaderLength(moduleIndex, portIndex, 1024);
                        }
                        if (!addStream(moduleIndex,portIndex,importDataList[i]))
                        {
                            ReleaseImportDataDlg(importDlg);
                            return;
                        }
                        xt_msleep(500);
                    }
                }

                int item = 0;
                Q_FOREACH(ImportData* s, importDataList) {
                    if (s) {
                        delete s;
                    }
                    item++;
                }

                importDataList.clear();
#endif
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

void ChassisViewWidget::getPortConfigInfo(int moduleIndex, int portIndex)
{
    // 更新端口的相关信息
    int maxLen;
    m_pXenaTester->getPortMaxHeaderLength(moduleIndex,portIndex,maxLen);

    int limitMs;
    m_pXenaTester->getPortTimeLimit(moduleIndex,portIndex,limitMs);

    QString mode;
    m_pXenaTester->portTxMode(moduleIndex,portIndex,mode);

    float fraction;
    m_pXenaTester->portRateFraction(moduleIndex,portIndex,fraction);

    int packetRate;
    m_pXenaTester->portPacketRate(moduleIndex,portIndex,packetRate);

    double bitRate;
    m_pXenaTester->portBitRate(moduleIndex,portIndex,bitRate);
}

bool ChassisViewWidget::addStream(int moduleIndex, int portIndex, ImportData* importData)
{
    if (!m_pXenaTester->addStream(moduleIndex, portIndex, importData)) {
        return false;
    }

    QStandardItem *portItem = getPortItem(moduleIndex,portIndex);
    CPort * port = m_pChassis->module(moduleIndex)->port(portIndex);
    /*
     *  在树形控件中增加流内容
     */
    int streamIndex = port->streamCount() - 1;
    QString streamName = port->stream(streamIndex)->streamName();
    QStandardItem *streamItem = new QStandardItem(streamName);
    streamItem->setData(STREAMSELECTION);
    portItem->appendRow(streamItem);
    setSiblingDataRole(portItem,STREAMSELECTION,streamIndex);

    return true;
}

bool ChassisViewWidget::addStream(int moduleIndex, int portIndex, CStream *stream)
{
    int sIndex = stream->itemOfPort();
    if (m_pXenaTester->addStream(moduleIndex, portIndex,sIndex, stream->data())) {
        CPort * port = m_pChassis->module(moduleIndex)->port(portIndex);
        int modifierCount = stream->modifierDataCount();
        for (int i = 0;i < modifierCount;i++)
        {
            if (m_pXenaTester->setStreamModifierCount(moduleIndex,portIndex,sIndex,modifierCount - 1))
            {
                if (addMidifier(moduleIndex,portIndex,sIndex,i,stream->modifierData(i)))
                {
                    for (int j = 0;j < port->streamCount();j++)
                    {
                        if (sIndex == port->stream(j)->itemOfPort())
                        {
                            // 跳变域内容的添加和配置
                            CModifierDialog::ModifierData * modifierData = new CModifierDialog::ModifierData();
                            modifierData->setPos(stream->modifierData(i)->getPos());
                            modifierData->setBitsNum(stream->modifierData(i)->getBitsNum());
                            modifierData->setMaxVal(stream->modifierData(i)->getMaxVal());
                            modifierData->setMinVal(stream->modifierData(i)->getMinVal());
                            modifierData->setModifyType(stream->modifierData(i)->getModifyType());
                            modifierData->setRepeatCnt(stream->modifierData(i)->getRepeatCnt());
                            modifierData->setStep(stream->modifierData(i)->getStep());
                            port->stream(j)->appendModifierData(modifierData);
                        }
                    }
                }
            }
        }

        /*
         *  在树形控件中增加流内容
         */
        int streamIndex = port->streamCount() - 1;
        QStandardItem *portItem = getPortItem(moduleIndex,portIndex);
        QString streamName = port->stream(streamIndex)->streamName();
        QStandardItem *streamItem = new QStandardItem(streamName);
        streamItem->setData(STREAMSELECTION);
        portItem->appendRow(streamItem);
        setSiblingDataRole(portItem,STREAMSELECTION,streamIndex);

        return true;
    }

    return false;
}

bool ChassisViewWidget::addStreams(int mIndex, int pIndex, CPort *port)
{
    int streamCount = port->streamCount();
    for (int i = 0;i < streamCount;i++)
    {
        if (!addStream(mIndex,pIndex,port->stream(i))){
            return false;
        }
        configStream(mIndex,pIndex,i,port->stream(i));              // 配置流内容
        xt_msleep(500);
    }

    return true;
}

bool ChassisViewWidget::delStream(int moduleIndex, int portIndex, int streamIndex)
{
    int mIndex = moduleIndex;
    int pIndex = portIndex;
    int sIndex = streamIndex;

    CStream *stream = m_pChassis->port(mIndex, pIndex)->stream(sIndex);
    // 对应的删除指令
    if (m_pXenaTester->delStream(mIndex, pIndex, stream))
    {
        if (m_pXenaTester->setStreamModifierCount(mIndex,pIndex,sIndex,-1))
        {
            // 删除树形控件中的内容
            QStandardItem *portItem = getPortItem(moduleIndex,portIndex);
            portItem->removeRow(sIndex);

            delete stream;

            return true;
        }

        return false;
    }
    else {
        QMessageBox::information(this, tr("Error"), tr("delete stream failed!"));

        return false;
    }
}

bool ChassisViewWidget::delStream(CStream *stream)
{
    CPort *port = stream->port();
    int sIndex = port->findStream(stream);

    CModule *module = port->module();
    int pIndex = module->findPort(port);

    int mIndex = m_pChassis->findModule(module);

    return delStream(mIndex, pIndex, sIndex);
}

bool ChassisViewWidget::addMidifier(int moduleIndex, int PortIndex, int streamIndex, int modifierIndex, CModifierDialog::ModifierData * modifierData)
{
    S_MODIFY modify;
    modify.modifyIndex = modifierIndex;
    modify.pos = modifierData->getPos();
    modify.minVal = modifierData->getMinVal();
    modify.step = modifierData->getStep();
    modify.maxVal = modifierData->getMaxVal();
    modify.repeat = modifierData->getRepeatCnt();

    switch(modifierData->getModifyType())
    {
    case CModifierDialog::INCREMENT:
        {
            m_pXenaTester->setStreamModifierInc(moduleIndex,PortIndex,streamIndex,modify);
            break;
        }
    case CModifierDialog::DECREMENT:
        {
            m_pXenaTester->setStreamModifierDec(moduleIndex,PortIndex,streamIndex,modify);
            break;
        }
    case CModifierDialog::RANDOM:
        {
            m_pXenaTester->setStreamModifierRam(moduleIndex,PortIndex,streamIndex,modify);
            break;
        }
    default:
        break;
    }
    m_pXenaTester->setStreamModifierRange(moduleIndex,PortIndex,streamIndex,modify);

    return true;
}

bool ChassisViewWidget::configPort(int moduleIndex, int portIndex, CPort * port)
{
    QString mode = port->txMode();
    if (mode.contains("Normal",Qt::CaseInsensitive))
    {
        m_pXenaTester->setPortTxModeNormal(moduleIndex,portIndex);
    }
    else if (mode.contains("Strict",Qt::CaseInsensitive))
    {
        m_pXenaTester->setPortTxModeStrict(moduleIndex,portIndex);
    }
    else if (mode.contains("Sequential",Qt::CaseInsensitive))
    {
        m_pXenaTester->setPortTxModeSequential(moduleIndex,portIndex);
    }

    m_pXenaTester->setPortMaxHeaderLength(moduleIndex,portIndex,port->maxHeaderLength());
    m_pXenaTester->setPortTimeLimit(moduleIndex,portIndex,port->txTimeLimit());
    m_pXenaTester->setPortRateFraction(moduleIndex,portIndex,port->rateFraction());
    m_pXenaTester->setPortPacketRate(moduleIndex,portIndex,port->packetRate());
    m_pXenaTester->setPortBitRate(moduleIndex,portIndex,port->bitRate());

    return true;
}

bool ChassisViewWidget::configStream(int moduleIndex, int portIndex, int streamIndex, CStream *stream_)
{
    CStream *stream = m_pChassis->module(moduleIndex)->port(portIndex)->stream(streamIndex);
    stream->setItemOfPort(stream_->itemOfPort());
    stream->setSettingsType(stream_->settingsType());
    stream->setTxPercent(stream_->txPercent());
    stream->setTxFramesPerSecond(stream_->txFramesPerSecond());
    stream->enableTx(stream_->isEnableTx());
    stream->setStreamName(stream_->streamName());
    stream->setStrategyType(stream_->strategy());
    stream->setStartPercent(stream_->startPercent());
    stream->setEndPercent(stream_->endPercent());
    stream->setStepPercent(stream_->stepPercent());
    stream->setStartFramesPerSec(stream_->startFramesPerSec());
    stream->setEndFramesPerSec(stream_->endFramesPerSec());
    stream->setStepFramesPerSec(stream_->stepFramesPerSec());
    stream->setLoopCnt(stream_->loopCnt());
    stream->setPacketLimit(stream_->packetLimit());

    return true;
}

bool ChassisViewWidget::onImportPortConf(int moduleIndex, int portIndex, QString fileName)
{
    // 当前选中的端口
    CPort * port = m_pChassis->module(moduleIndex)->port(portIndex);

    for (int i = port->streamCount() - 1;i >= 0;i--)
    {
        delStream(moduleIndex,portIndex,i);
    }

    CPort * importPort = new CPort(); // 导入文件后的端口
    if (!m_pXenaTester->importPort(fileName,moduleIndex,portIndex,importPort))
    {
        return false;
    }

    configPort(moduleIndex,portIndex,importPort);

    addStreams(moduleIndex,portIndex,importPort);

    delete importPort;
    return true;
}

bool ChassisViewWidget::onImportModuleConf(int moduleIndex, QString fileName)
{
    CModule *module = m_pChassis->module(moduleIndex);
    // 是否要删除端口？

    CModule *importModule = new CModule();
    if (!m_pXenaTester->importModule(fileName,moduleIndex,importModule))
    {
        return false;
    }

clearPort:
    for (int portIndex = 0;portIndex < module->portCount();portIndex++)
    {
        CPort *port = module->port(portIndex);

        for (int i = port->streamCount() - 1;i >= 0;i--)
        {
            if (m_pXenaTester->reservePort(moduleIndex,portIndex))
            {
                delStream(moduleIndex,portIndex,i);
                xt_msleep(500);
            }
        }
    }

    for (int portIndex = 0;portIndex < module->portCount();portIndex++)
    {
        CPort *port = module->port(portIndex);

        for (int i = port->streamCount() - 1;i >= 0;i--)
        {
            if (m_pXenaTester->reservePort(moduleIndex,portIndex))
            {
                goto clearPort;
            }
        }
    }

    for (int portIndex = 0;portIndex < module->portCount();portIndex++)
    {
        CPort * importPort = importModule->port(portIndex);
        if (importPort->streamCount() <= 0)
        {
            continue;
        }

        if (m_pXenaTester->reservePort(moduleIndex,portIndex))
        {
            configPort(moduleIndex,portIndex,importPort);

            addStreams(moduleIndex,portIndex,importPort);
        }
        xt_msleep(200);
    }

    for (int portIndex = 0;portIndex < module->portCount();portIndex++)
    {
        CPort * importPort = importModule->port(portIndex);
        if (importPort->streamCount() != module->port(portIndex)->streamCount())
        {
            goto clearPort;
        }
    }

    delete importModule;
    return true;
}

void ChassisViewWidget::updateChassisOwner()
{
    QModelIndex chassisOwnerIndex = m_pChassisModel->index(0,OWNERINDEX,QModelIndex());
    m_pChassisModel->setData(chassisOwnerIndex,m_pChassis->reservedBy());
}

void ChassisViewWidget::updateModuleOwner(int moduleIndex)
{
    QModelIndex chassisIndex = m_pChassisModel->index(0,0,QModelIndex());

    QModelIndex moduleOwnerModelIndex = m_pChassisModel->index(moduleIndex,OWNERINDEX,chassisIndex);
    m_pChassisModel->setData(moduleOwnerModelIndex,m_pChassis->module(moduleIndex)->reservedBy());
}

void ChassisViewWidget::updatePortName(int moduleIndex,int portIndex)
{
    QModelIndex chassisIndex = m_pChassisModel->index(0,0,QModelIndex());
    QModelIndex moduleModelIndex = m_pChassisModel->index(moduleIndex,0,chassisIndex);
    QModelIndex portNameModelIndex = m_pChassisModel->index(portIndex,NAMEINDEX,moduleModelIndex);

    QString oldPortName = m_pChassisModel->data(portNameModelIndex).toString();

    QString portName = m_pChassis->module(moduleIndex)->port(portIndex)->portName();

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

    bool linked = m_pChassis->module(moduleIndex)->port(portIndex)->linked();
    bool trafficed = m_pChassis->module(moduleIndex)->port(portIndex)->actived();
    QString syncStatus,trafficStatus;
    QModelIndex portSyncModelIndex = m_pChassisModel->index(portIndex,SYNCINDEX,moduleModelIndex);
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

    QString portOwner = m_pChassis->module(moduleIndex)->port(portIndex)->reservedBy();
    QModelIndex portOwnerModelIndex = m_pChassisModel->index(portIndex,OWNERINDEX,moduleModelIndex);
    m_pChassisModel->setData(portOwnerModelIndex,portOwner);

    QStandardItem *portNameItem = m_pChassisModel->itemFromIndex(portOwnerModelIndex.sibling(portOwnerModelIndex.row(),0));

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
            portNameItem->setForeground(brush);
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

QStandardItem* ChassisViewWidget::getPortItem(int moduleIndex, int portIndex)
{
    QModelIndex chassisParentIndex = m_pChassisModel->index(0,0,QModelIndex());                                         // 获取父节点（chassis）
    QModelIndex moduleChildIndex = m_pChassisModel->index(moduleIndex,0,chassisParentIndex);                            // 获取子节点（module）
    QModelIndex portChildIndex = m_pChassisModel->index(portIndex,0,moduleChildIndex);                                  // 获取孙子节点（port）

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

