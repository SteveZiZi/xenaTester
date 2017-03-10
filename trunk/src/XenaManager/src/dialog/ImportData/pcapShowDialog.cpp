#include "pcapShowDialog.h"

#include <QtGui>
#include <QDir>
#include <QDebug>

#include "XenaManager/src/service/fileAnalysis/pcapAnalysis/pcapAnalysis.h"

static const int CB_WIDTH[CPcapShowDialog::MAX_COL_NUM] = {
    40,
    80,
    60,
    160,
    80
};

static QString frameTypeToString(enumFrameType type)
{
    switch(type) {
    case FRAME_TYPE_NET_GOOSE:
        return "GOOSE";
    case FRAME_TYPE_NET_9_2:
        return "SMV92";
    default:
        return "Unknown";
    }

    return "Unknown";
}


CPcapShowDialog::CPcapShowDialog(QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
: ImportDataDialog(parent,flags)
, m_pcapFileName("")
, m_selectedItem(-1)
{
    m_analysis = CPcapAnalysis::getInstance();
    this->resize(450, 500);
}

CPcapShowDialog::~CPcapShowDialog()
{
    cleaerResource();
}

void CPcapShowDialog::cleaerResource()
{
    m_pcapFileName = "";
}

bool CPcapShowDialog::parseFile(const QString & fileName)
{
    bool rc = false;
    m_pcapFileName = fileName;

    if (!m_analysis->parseFile(m_pcapFileName)) {
        goto exitCode;
    }

    _setupUI();

    m_tableWidget->clearContents();
    m_tableWidget->setRowCount(m_analysis->m_frame.size());
    int row = 0;
    Q_FOREACH(CPcapAnalysis::Frame *frame, m_analysis->m_frame)
    {
        QString text;
        m_tableWidget->setItem(row, COL_ITEM, new QTableWidgetItem(QString::number(row+1)));
        if (frameTypeToString(frame->type) == "Unknown") {
            text = "--";
        }
        else {
            text = QString("%1").arg(frame->appId, 4, 16, QChar('0'));
        }
        m_tableWidget->setItem(row, COL_APPID, new QTableWidgetItem(text.toUpper()));
        text = QString("%1-%2-%3-%4-%5-%6") 
                        .arg(frame->mac[0], 2, 16, QChar('0'))
                        .arg(frame->mac[1], 2, 16, QChar('0'))
                        .arg(frame->mac[2], 2, 16, QChar('0'))
                        .arg(frame->mac[3], 2, 16, QChar('0'))
                        .arg(frame->mac[4], 2, 16, QChar('0'))
                        .arg(frame->mac[5], 2, 16, QChar('0'));
        m_tableWidget->setItem(row, COL_MAC_ADDR, new QTableWidgetItem(text.toUpper()));
        m_tableWidget->setItem(row, COL_LENGTH, new QTableWidgetItem(QString::number(frame->len)));
        m_tableWidget->setItem(row, COL_TYPE, new QTableWidgetItem(frameTypeToString(frame->type)));

        for(int i = 0; i < MAX_COL_NUM; i++)
            m_tableWidget->item(row, i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        row++;
    }

    rc = true;
exitCode:
    if (!rc) {
        cleaerResource();
    }

    return rc;
}

void CPcapShowDialog::_setupUI()
{
    m_tableWidget = createTableWidget();
    m_tableWidget->setMinimumWidth(100);

    m_labelFileName = new QLabel(m_pcapFileName, this);

    _layout();

    //Add Menu
    m_selectionAction = new QAction(tr("Import"),this);
    connect(m_selectionAction,SIGNAL(triggered()),this,SLOT(exprotCB()));
    m_tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tableWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(onCustomContextMenuRequested(const QPoint&)));
}

void CPcapShowDialog::_layout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_labelFileName);
    mainLayout->addWidget(m_tableWidget);

    mainLayout->setMargin(1);
    this->setLayout(mainLayout);
}

QTableWidget* CPcapShowDialog::createTableWidget()
{
    const QString captionName[MAX_COL_NUM] = {
        CPcapShowDialog::tr("Num"),
        CPcapShowDialog::tr("Type"),
        CPcapShowDialog::tr("APPID"),
        CPcapShowDialog::tr("MAC"),
        CPcapShowDialog::tr("Length")
    };

    QTableWidget* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(MAX_COL_NUM);

    tableWidget->verticalHeader()->hide();

    for(int i = 0; i < MAX_COL_NUM; i++) {
        QTableWidgetItem *headerItem = new QTableWidgetItem(captionName[i]);
        tableWidget->setHorizontalHeaderItem(i, headerItem);
        tableWidget->setColumnWidth(i, CB_WIDTH[i]);
    }

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    return tableWidget;
}

void CPcapShowDialog::exprotCB()
{
    m_lSelectedItemList.clear();
    for(int i = 0;i < m_tableWidget->rowCount();i++)
    {
        for(int j = 0;j < m_tableWidget->columnCount();j++)
        {
            if (m_tableWidget->item(i,j)->isSelected())
            {
                m_lSelectedItemList.push_back(i);
                break;
            }
        }
    }

    if (m_lSelectedItemList.size() >= 0) {
        this->accept();
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("No Selection Packets"));
    }
}

void CPcapShowDialog::onCustomContextMenuRequested(const QPoint &point)
{
    qDebug() << "onCustomContextMenuRequested";

    QTableWidgetItem* item = m_tableWidget->itemAt(point);
    if (!item) {
        return;
    }

    QMenu contextMenu;
    contextMenu.addAction(m_selectionAction);
    contextMenu.exec(QCursor::pos());  
}


bool CPcapShowDialog::selectedFrame(ImportData &data)
{
    Q_ASSERT(m_selectedItem != -1 && m_analysis->m_frame.size() > m_selectedItem);

    if (!data.frame || MAX_FRAME_SIZE < m_analysis->m_frame[m_selectedItem]->len) {
        return false;
    }

    data.frameLength = m_analysis->m_frame[m_selectedItem]->len;
    memcpy(data.frame, m_analysis->m_frame[m_selectedItem]->data, data.frameLength);

    data.appId = m_analysis->m_frame[m_selectedItem]->appId;
    switch (m_analysis->m_frame[m_selectedItem]->type) {
    case FRAME_TYPE_NET_9_2:
        data.frameStyle = "SMV92";
        break;
    case FRAME_TYPE_NET_GOOSE:
        data.frameStyle = "GOOSE";
        break;
    default:
        break;
    }

    return true;
}

bool CPcapShowDialog::selectedFrame(QList<ImportData*> &dataList)
{
    Q_ASSERT(m_lSelectedItemList.size() > 0 && m_lSelectedItemList.size() < m_analysis->m_frame.size());

    dataList.clear();

    for (int i = 0;i < m_lSelectedItemList.size();i++)
    {
        ImportData *data = new ImportData;
        m_selectedItem = m_lSelectedItemList[i];
        selectedFrame(*data);

        dataList.append(data);
    }

    return true;
}