#include "SegmentViewWidget.h"
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"
#include "XenaManager/src/dialog/configDialog/streamconfigwidget.h"
#include "XenaManager/src/common_ctrl/ethSegEdit/ethSegEditDelegate.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/device.h"
#include "XenaManager/src/Communicate/stream.h"
#include <QMenu>
#include <QAction>
#include <QTreeView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QLayout>
#include <QDebug>


#define SEGMENTMINWIDTH 200
#define VALUEMINWIDTH 400


SegmentViewWidget::SegmentViewWidget(CXenaTester *xenaTester,QWidget *parent)
	: QWidget(parent),
    m_pXenaTester(xenaTester)
{
	initUI();
}

SegmentViewWidget::~SegmentViewWidget()
{

}

void SegmentViewWidget::updateSegment()
{
    updateCurrentIndex();

    updateStream();

    m_importData = *(m_pStream->data());

    updateView();

    updateModifier();
}

void SegmentViewWidget::onAddModifierAction()
{
    eth_segment_info info;
    QStandardItem *item = m_pSegmentModel->itemFromIndex(m_currentIndex);

    if (!ethGetSegmentInfo(item,info))
    {
        return;
    }

    CModifierDialog modifierDialog(info,this,0);
    int result = modifierDialog.exec();
    if (QDialog::Accepted == result)
    {
        CModifierDialog::ModifierData *modifierData = new CModifierDialog::ModifierData;
        modifierDialog.modifierData(*modifierData);

        int modifyIndex = m_pStreamWidget->currentStreamModifierCnt();

        // 增加跳变域的指令
        m_pXenaTester->device()->setStreamModifierCount(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modifyIndex);
        setModifierCommand(modifierData,modifyIndex);

        // 在树形控件中增加跳变域内容
        addModifier(modifierData,modifyIndex,m_currentIndex);

        // 将跳变域内容增加到CStream中
        m_pStream->appendModifierData(modifierData);
    }
}

void SegmentViewWidget::onDelModifierAction()
{
    int currentModifierCount = m_pStreamWidget->currentStreamModifierCnt();
    currentModifierCount -= 2;                      // 因setStreamModifierCount传入的参数是index 所以要减2
    m_pXenaTester->device()->setStreamModifierCount(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,currentModifierCount);


    // 删除CStream中的跳变域内容
    QModelIndex index = m_currentIndex.sibling(m_currentIndex.row(),0);
    QString modifierText = m_pSegmentModel->itemFromIndex(index)->text();
    if (modifierText.contains("Modifier"))
    {
        int modifierIndex = modifierText.remove(" ").toUInt();
        CModifierDialog::ModifierData *modifierData = m_pStream->modifierData(modifierIndex);
        if (modifierData)
        {
            m_pStream->removeModifierData(modifierData);
            delete modifierData;
        }
    }

    // 删除控件中的内容
    QModelIndex parentIndex = m_currentIndex.parent();
    QStandardItem *item = m_pSegmentModel->itemFromIndex(parentIndex);
    item->removeRow(m_currentIndex.row());
}

void SegmentViewWidget::onEditModifierAction()
{
    QStandardItem *item = m_pSegmentModel->itemFromIndex(m_currentIndex.sibling(m_currentIndex.row(),0));
    int modifierIndex = item->text().remove("Modifier ").toUInt();

    CModifierDialog::ModifierData *modifierData = m_pStream->modifierData(modifierIndex);
    
    CModifierDialog modifierDialog(*modifierData,this,0);

    int result = modifierDialog.exec();
    if (QDialog::Accepted == result)
    {
        // 删除原先CStream中的跳变域的内容
        m_pStream->removeModifierData(modifierData);
        delete modifierData;

        // 用新的内容进行替换
        CModifierDialog::ModifierData *modifierData = new CModifierDialog::ModifierData;
        modifierDialog.modifierData(*modifierData);

        setModifierCommand(modifierData,modifierIndex);

        // 将跳变域内容增加到CStream中
        m_pStream->appendModifierData(modifierData);

        QString modifierItemData = createModifierItemData(*modifierData);
        m_pSegmentModel->itemFromIndex(m_currentIndex)->setText(modifierItemData);
    }
}

void SegmentViewWidget::onCustomContextMenuRequested(const QPoint &point)
{
    m_currentIndex = m_pSegmentTreeView->indexAt(point);
    if (m_currentIndex.column() < 1)
    {
        return;
    }

    QMenu contextMenu;
    
    QModelIndex index = m_currentIndex.sibling(m_currentIndex.row(),0);
    QString itemText = m_pSegmentModel->itemFromIndex(index)->text();

    if (itemText.contains("Modifier",Qt::CaseInsensitive))
    {
        contextMenu.addAction(m_pEditModifierAction);
        contextMenu.addAction(m_pDelModifierAction);
    }
    else
    {
        if (m_pStreamWidget->currentStreamModifierCnt() < 
            m_pStreamWidget->currentStreamMaxModifierCnt())
        {
            m_pAddModifierAction->setEnabled(true);
        }
        else
        {
            m_pAddModifierAction->setEnabled(false);
        }

        contextMenu.addAction(m_pAddModifierAction);
    }

    contextMenu.exec(QCursor::pos());
}

void SegmentViewWidget::initUI()
{
	m_pSegmentTreeView = new QTreeView(this);
	//m_pSegmentTreeView->header()->setStretchLastSection(true);
	m_pSegmentModel = new QStandardItemModel(this);
	m_pSegmentTreeView->setModel(m_pSegmentModel);

    m_pSegmentTreeView->setItemDelegate(new CEthSegEditDelegate(m_pSegmentTreeView)); // 智能输入编辑托管

// 	QStringList segmentStringList;
// 	segmentStringList<< tr("字段") << tr("数据");
// 	m_pSegmentModel->setHorizontalHeaderLabels(segmentStringList);

    createContextMenu();

	setMainLayout();
}

void SegmentViewWidget::setMainLayout()
{
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(m_pSegmentTreeView);
	mainLayout->setMargin(1);
	this->setLayout(mainLayout);
}

void SegmentViewWidget::setSegmentData()
{
	QStandardItem *item = new QStandardItem("");
	m_pSegmentModel->appendRow(item);
	m_pSegmentModel->setItem(0,1,new QStandardItem("Null"));
}

void SegmentViewWidget::updateView()
{
	m_pSegmentModel->clear();
    if (ethFrameToItemModel(m_importData.frame, m_importData.frameLength, m_pSegmentModel)) {
        emit updateStreamLength(m_importData.frameLength);
        m_pSegmentTreeView->setColumnWidth(0, SEGMENTMINWIDTH);
        m_pSegmentTreeView->setColumnWidth(1, VALUEMINWIDTH);
        m_pSegmentTreeView->expandAll();
    }
}

void SegmentViewWidget::updateCurrentIndex()
{
    m_pStreamWidget = qobject_cast<StreamConfigWidget *>(this->parent());

    m_currentModuleIndex = m_pStreamWidget->currentModuleIndex();
    m_currentPortIndex = m_pStreamWidget->currentPortIndex();
    m_currentStreamIndex = m_pStreamWidget->currentStreamIndex();
}

void SegmentViewWidget::updateStream()
{
    m_pStream = m_pStreamWidget->currentStream();
}

void SegmentViewWidget::updateModifier()
{
    if (m_pStream->modifierDataCount())
    {
        for (int modifierNum = 0;modifierNum < m_pStream->modifierDataCount();modifierNum++)
        {
            CModifierDialog::ModifierData * modifierData = m_pStream->modifierData(modifierNum);

            addModifier(modifierData,modifierNum);
        }
    }
}

void SegmentViewWidget::createActions()
{
    m_pAddModifierAction = new QAction(tr("增加跳变域"),this);
    connect(m_pAddModifierAction,SIGNAL(triggered()),this,SLOT(onAddModifierAction()));

    m_pDelModifierAction = new QAction(tr("删除跳变域"),this);
    connect(m_pDelModifierAction,SIGNAL(triggered()),this,SLOT(onDelModifierAction()));

    m_pEditModifierAction = new QAction(tr("编辑跳变域"),this);
    connect(m_pEditModifierAction,SIGNAL(triggered()),this,SLOT(onEditModifierAction()));
}

void SegmentViewWidget::createContextMenu()
{
    createActions();
    m_pSegmentTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_pSegmentTreeView,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(onCustomContextMenuRequested(const QPoint&)));
}

void SegmentViewWidget::setModifierCommand(CModifierDialog::ModifierData *modifierData,int modifierIndex)
{
    S_MODIFY modify;
    modify.modifyIndex = modifierIndex;
    modify.pos = modifierData->pos;
    modify.minVal = modifierData->minVal;
    modify.step = modifierData->step;
    modify.maxVal = modifierData->maxVal;
    modify.repeat = modifierData->repeatCnt;

    switch(modifierData->type)
    {
    case CModifierDialog::INCREMENT:
        {
            m_pXenaTester->device()->setStreamModifierInc(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
            break;
        }
    case CModifierDialog::DECREMENT:
        {
            m_pXenaTester->device()->setStreamModifierDec(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
            break;
        }
    case CModifierDialog::RANDOM:
        {
            m_pXenaTester->device()->setStreamModifierRam(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
            break;
        }
    default:
        break;
    }
    m_pXenaTester->device()->setStreamModifierRange(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
}

void SegmentViewWidget::addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex,QModelIndex & modelIndex)
{
    QString modifierItemData = createModifierItemData(*modifierData);

    QModelIndex index = modelIndex.sibling(modelIndex.row(),0);
    if (index.isValid())
    {
        QStandardItem *item = m_pSegmentModel->itemFromIndex(index);

        QStandardItem *modifierItem = new QStandardItem("Modifier " + QString::number(modifierIndex));
        modifierItem->setEditable(false);
        item->appendRow(modifierItem);

        QStandardItem *modifierDataItem = new QStandardItem(modifierItemData);
        modifierDataItem->setEditable(false);
        item->setChild(item->rowCount() - 1,1,modifierDataItem);
    }
}

void SegmentViewWidget::addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex)
{
    QString modifierItemData = createModifierItemData(*modifierData);


    for (int row = 0;row < m_pSegmentModel->rowCount();row++)
    {
        QStandardItem *item = m_pSegmentModel->item(row, 1);
        item = ethFindSegmentItem(m_pSegmentModel, item, modifierData->pos);
        if (item) {
            QStandardItem *modifierItem = new QStandardItem("Modifier " + QString::number(modifierIndex));
            modifierItem->setEditable(false);
            item->appendRow(modifierItem);

            QStandardItem *modifierDataItem = new QStandardItem(modifierItemData);
            modifierDataItem->setEditable(false);
            item->setChild(item->rowCount() - 1,1,modifierDataItem);

            break;
        }
    }
}

QString SegmentViewWidget::createModifierItemData(CModifierDialog::ModifierData modifierData)
{
    QString modifierItemData = "P:";
    modifierItemData += QString::number(modifierData.pos);
    modifierItemData += ",FFFF,";
    if (CModifierDialog::INCREMENT == modifierData.type)
    {
        modifierItemData += "INC,";
    }
    else if (CModifierDialog::DECREMENT == modifierData.type)
    {
        modifierItemData += "DEC,";
    }
    else if (CModifierDialog::RANDOM == modifierData.type)
    {
        modifierItemData += "RAM,";
    }
    modifierItemData += "R:";
    modifierItemData += QString::number(modifierData.repeatCnt);
    modifierItemData += "[";
    modifierItemData += QString::number(modifierData.minVal);
    modifierItemData += ":";
    modifierItemData += QString::number(modifierData.maxVal);
    modifierItemData += "],S";
    modifierItemData += QString::number(modifierData.step);

    return modifierItemData;
}