#include "SegmentViewWidget.h"
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"
#include "XenaManager/src/dialog/configDialog/streamconfigwidget.h"
#include "XenaManager/src/common_ctrl/ethSegEdit/ethSegEditDelegate.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/stream.h"
#include "XenaManager/src/common_ctrl/frameShowWidget/frameShowWidget.h"
#include <QMenu>
#include <QAction>
#include <QTreeView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QLayout>
#include <QDebug>
#include <QMessageBox>
#include <QSplitter>


#define SEGMENT_MIN_WIDTH 200
#define VALUE_MIN_WIDTH 400


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
    disconnect(m_pSegmentModel,SIGNAL(itemChanged(QStandardItem *)),this,SLOT(onItemChanged(QStandardItem *)));

    updateCurrentIndex();

    updateStream();

    m_importData = *(m_pStream->data());

    updateView();

    updateModifier();

    connect(m_pSegmentModel,SIGNAL(itemChanged(QStandardItem *)),this,SLOT(onItemChanged(QStandardItem *)));
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

        int modifyIndex = m_pStream->modifierCount();
        // 增加跳变域的指令
        m_pXenaTester->setStreamModifierCount(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modifyIndex);
        setModifierCommand(modifierData,modifyIndex);

        // 在树形控件中增加跳变域内容
        addModifier(modifierData,modifyIndex,m_currentIndex);

        // 将跳变域内容增加到CStream中
        m_pStream->appendModifierData(modifierData);
        m_pStream->setModifierCount(modifyIndex + 1);
    }
}

void SegmentViewWidget::onDelModifierAction()
{
    QString modifierDataStr = m_pSegmentModel->itemFromIndex(m_currentIndex)->text();

    int modifierIndex = m_pStream->itemOfModiferData(parseModifierItemData(modifierDataStr));

//     bool execCom = false;                                               // 判断是否先删掉了索引数大的跳变域
//     for (int i = 0;i < m_pStream->modifierCount();i++)
//     {
//         if (modifierIndex > i)
//         {
//             execCom = true;
//             break;
//         }
//     }

    int currentModifierCount = m_pStream->modifierCount();
    currentModifierCount -= 2;                      // setStreamModifierCount中传入的是index，会执行+1操作，所以要-2
    if (m_pXenaTester->setStreamModifierCount(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,currentModifierCount))
    {
        removeModifier();
        for (int i = 0;i < m_pStream->modifierCount();i++)
        {
            setModifierCommand(m_pStream->modifierData(i),i);
        }
    }
}

void SegmentViewWidget::onEditModifierAction()
{
    QStandardItem *item = m_pSegmentModel->itemFromIndex(m_currentIndex.sibling(m_currentIndex.row(),0));
    QString modifierDataStr = m_pSegmentModel->itemFromIndex(m_currentIndex)->text();

    int modifierIndex = m_pStream->itemOfModiferData(parseModifierItemData(modifierDataStr));

    if (-1 != modifierIndex)
    {
        CModifierDialog::ModifierData *modifierData = m_pStream->modifierData(modifierIndex);

        eth_segment_info info;
        {
            QModelIndex index = item->parent()->index();
            QModelIndex segInfoIndex = m_pSegmentModel->index(index.row(), 1, index.parent());
            QStandardItem *segInfoItem = m_pSegmentModel->itemFromIndex(segInfoIndex);
            ethGetSegmentInfo(segInfoItem, info);
        }
        
        CModifierDialog modifierDialog(*modifierData, info, this);

        int result = modifierDialog.exec();
        if (QDialog::Accepted == result)
        {
            int len = modifierData->bitsNum==CModifierDialog::MODIFY_16BITS ? 2 : 1;
            // 删除原先CStream中的跳变域的内容
            m_pFrameShowWidget->removeHighLight(modifierData->pos, len);
            m_pFrameShowWidget->clearSelected();

            // 用新的内容进行替换
            modifierDialog.modifierData(*modifierData);
            setModifierCommand(modifierData,modifierIndex);

            len = modifierData->bitsNum==CModifierDialog::MODIFY_16BITS ? 2 : 1;
            QString modifierItemData = createModifierItemData(*modifierData);
            m_pSegmentModel->itemFromIndex(m_currentIndex)->setText(modifierItemData);
            m_pSegmentModel->itemFromIndex(m_currentIndex)->setData(QVariant(eth_segment_info(modifierData->pos, len).data));

            m_pFrameShowWidget->appendHighLight(modifierData->pos, len);
            m_pFrameShowWidget->setSelected(modifierData->pos, len);
        }
    }
    else
    {
        QMessageBox::warning(0,tr("Warning"),tr("Modifier index Error!"),QString(tr("OK")));
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
        contextMenu.exec(QCursor::pos());
    }
    else
    {
        if (!m_pSegmentModel->itemFromIndex(m_currentIndex)->text().isEmpty())
        {
            if (m_pStream->modifierCount() < 
                m_pStream->maxModifierCount())
            {
                m_pAddModifierAction->setEnabled(true);
            }
            else
            {
                m_pAddModifierAction->setEnabled(false);
            }

            contextMenu.addAction(m_pAddModifierAction);
            contextMenu.exec(QCursor::pos());
        }
    }
}

void SegmentViewWidget::onItemChanged(QStandardItem * item)
{
    QString itemData = item->text();
    if (!itemData.isEmpty())
    {
        eth_segment_info info;
        ethGetSegmentInfo(item,info);

        QStringList hexDataList = itemData.split(" ");
        if (hexDataList.size() == info.length) {
            for (int index = 0;index < info.length; index++) {
                m_importData.frame[index+info.pos] = hexDataList[index].toInt(0, 16);
            }
            m_pFrameShowWidget->updateFrame(info.pos, info.length, m_importData.frame+info.pos);
            m_pStream->setData(m_importData);
            m_pXenaTester->updateStreamHeader(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,m_pStream);
        }
    }
}

void SegmentViewWidget::itemSelected(const QModelIndex &index)
{
    QStandardItem *item = m_pSegmentModel->itemFromIndex(index);
    if (index.column() != 1) {
        QModelIndex siblingIndex = m_pSegmentModel->index(index.row(), 1, index.parent());
        item = m_pSegmentModel->itemFromIndex(siblingIndex);
    }

    m_pFrameShowWidget->clearSelected();
    eth_segment_info info;
    if (ethGetSegmentInfo(item, info)) {
        m_pFrameShowWidget->setSelected(info.pos, info.length);
    }
}

void SegmentViewWidget::initUI()
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

	m_pSegmentTreeView = new QTreeView(splitter);
	m_pSegmentModel = new QStandardItemModel(this);
	m_pSegmentTreeView->setModel(m_pSegmentModel);
    m_pSegmentTreeView->setItemDelegate(new CEthSegEditDelegate(m_pSegmentTreeView)); // 智能输入编辑托管
    m_pSegmentTreeView->setMinimumWidth(300);

    m_pFrameShowWidget = new CFrameShowWidget(splitter);
    m_pFrameShowWidget->setMinimumWidth(330);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(splitter);
    mainLayout->setMargin(1);
    this->setLayout(mainLayout);

    connect(m_pSegmentTreeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(itemSelected(const QModelIndex &)));

    createContextMenu();
}

void SegmentViewWidget::setMainLayout()
{
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
        m_pSegmentTreeView->setColumnWidth(0, SEGMENT_MIN_WIDTH);
        m_pSegmentTreeView->setColumnWidth(1, VALUE_MIN_WIDTH);
        m_pSegmentTreeView->expandAll();
    }

    m_pFrameShowWidget->setFrame(m_importData.frame, m_importData.frameLength);
    m_pFrameShowWidget->clearSelected();
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
        m_pStream->setModifierCount(m_pStream->modifierDataCount());

        for (int modifierNum = 0;modifierNum < m_pStream->modifierDataCount();modifierNum++)
        {
            CModifierDialog::ModifierData * modifierData = m_pStream->modifierData(modifierNum);

            addModifier(modifierData,modifierNum);
        }
    }
    else
    {
        m_pStream->setModifierCount(0);
    }
}

void SegmentViewWidget::createActions()
{
    m_pAddModifierAction = new QAction(tr("Add Modifier"),this);
    connect(m_pAddModifierAction,SIGNAL(triggered()),this,SLOT(onAddModifierAction()));

    m_pDelModifierAction = new QAction(tr("Del Modifier"),this);
    connect(m_pDelModifierAction,SIGNAL(triggered()),this,SLOT(onDelModifierAction()));

    m_pEditModifierAction = new QAction(tr("Edit Modifier"),this);
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
            m_pXenaTester->setStreamModifierInc(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
            break;
        }
    case CModifierDialog::DECREMENT:
        {
            m_pXenaTester->setStreamModifierDec(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
            break;
        }
    case CModifierDialog::RANDOM:
        {
            m_pXenaTester->setStreamModifierRam(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
            break;
        }
    default:
        break;
    }
    m_pXenaTester->setStreamModifierRange(m_currentModuleIndex,m_currentPortIndex,m_currentStreamIndex,modify);
}

void SegmentViewWidget::addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex,QModelIndex & modelIndex)
{
    QString modifierItemData = createModifierItemData(*modifierData);

    QModelIndex index = modelIndex.sibling(modelIndex.row(),0);
    if (index.isValid())
    {
        QStandardItem *item = m_pSegmentModel->itemFromIndex(index);
        addModifier(modifierData, modifierIndex, item);
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
            addModifier(modifierData, modifierIndex, item);
            break;
        }
    }
    update();                                                           // 强制更新 防止下次遍历item时出错
}

void SegmentViewWidget::addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex, QStandardItem *item)
{
    if (item) {
        QStandardItem *modifierItem = new QStandardItem("Modifier " /*+ QString::number(modifierIndex)*/);
        modifierItem->setEditable(false);
        item->appendRow(modifierItem);

        QString modifierItemData = createModifierItemData(*modifierData);
        QStandardItem *modifierDataItem = new QStandardItem(modifierItemData);
        modifierDataItem->setEditable(false);
        eth_segment_info info(modifierData->pos, (modifierData->bitsNum==CModifierDialog::MODIFY_16BITS)?2:1);
        modifierDataItem->setData(QVariant(info.data));

        item->setChild(item->rowCount() - 1,1,modifierDataItem);

        // high light modifier
        m_pFrameShowWidget->appendHighLight(info.pos, info.length);
    }
}

void SegmentViewWidget::removeModifier()
{
    // 删除CStream中的跳变域内容
    QModelIndex index = m_currentIndex.sibling(m_currentIndex.row(),0);
    QString modifierText = m_pSegmentModel->itemFromIndex(index)->text();
    if (modifierText.contains("Modifier"))
    {
        QString modifierDataStr = m_pSegmentModel->itemFromIndex(m_currentIndex)->text();

        int modifierIndex = m_pStream->itemOfModiferData(parseModifierItemData(modifierDataStr));

        if (-1 != modifierIndex)
        {
            CModifierDialog::ModifierData *modifierData = m_pStream->modifierData(modifierIndex);
            if (modifierData)
            {
                m_pStream->removeModifierData(modifierData);
                m_pStream->setModifierCount(m_pStream->modifierCount() - 1);
                QModelIndex parentIndex = m_currentIndex.parent();
                QStandardItem *item = m_pSegmentModel->itemFromIndex(parentIndex);
                item->removeRow(m_currentIndex.row());
                m_pFrameShowWidget->removeHighLight(modifierData->pos, modifierData->bitsNum==CModifierDialog::MODIFY_16BITS?2:1);
                delete modifierData;
            }
        }
        else
        {
            QMessageBox::warning(0,tr("Warning"),tr("Modifier index Error!"),QString(tr("OK")));
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
    modifierItemData += ",T";
    if (CModifierDialog::MODIFY_8BITS == modifierData.bitsNum)
    {
        modifierItemData += "1";
    }
    else if (CModifierDialog::MODIFY_16BITS == modifierData.bitsNum)
    {
        modifierItemData += "2";
    }

    return modifierItemData;
}

static inline int getNumberFormString(const QString &str)
{
    QString tempStr;
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i].isDigit())
        {
            tempStr.append(str[i]);
        }
    }
    return tempStr.toUInt();
}

CModifierDialog::ModifierData SegmentViewWidget::parseModifierItemData(const QString & itemStr)
{
    CModifierDialog::ModifierData modifierData;
    QStringList modifierStrList = itemStr.split(",");
    modifierData.pos = getNumberFormString(modifierStrList[0]);

    if (modifierStrList[2].contains("INC",Qt::CaseInsensitive))
    {
        modifierData.type = CModifierDialog::INCREMENT;
    }
    else if (modifierStrList[2].contains("DEC",Qt::CaseInsensitive))
    {
        modifierData.type = CModifierDialog::DECREMENT;
    }
    else if (modifierStrList[2].contains("RAM",Qt::CaseInsensitive))
    {
        modifierData.type = CModifierDialog::RANDOM;
    }

    QStringList modifierRange = modifierStrList[3].split("[");
    modifierData.repeatCnt = getNumberFormString(modifierRange[0]);

    QStringList rangeStr = modifierRange[1].split(":");
    modifierData.minVal = getNumberFormString(rangeStr[0]);
    modifierData.maxVal = getNumberFormString(rangeStr[1]);

    modifierData.step = getNumberFormString(modifierStrList[4]);

    if (1 == getNumberFormString(modifierStrList[5]))
    {
        modifierData.bitsNum = CModifierDialog::MODIFY_8BITS;
    }
    else if (2 == getNumberFormString(modifierStrList[5]))
    {
        modifierData.bitsNum = CModifierDialog::MODIFY_16BITS;
    }

    return modifierData;
}