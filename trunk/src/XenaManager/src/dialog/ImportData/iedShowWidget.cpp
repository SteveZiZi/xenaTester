#include "iedShowWidget.h"

#include <QtGui/QTreeWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QDir>
#include <QDebug>
#include <QBoxLayout>

#include "CBTabWidget.h"

#include "XenaManager/src/service/fileAnalysis/scdAnalysis/scdAnalysis.h"

const int CH_COL_WIDTH[CIedShowWidget::CH_MAX_COLUMN] = {
    50, 50, 150, 150
};

CIedShowWidget::CIedShowWidget(QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
: ImportDataDialog(parent,flags)
, m_scdFileName("")
, m_curIed(NULL)
, m_cbFrame(NULL)
, m_length(0)
{
    this->resize(800, 600);
}

CIedShowWidget::~CIedShowWidget()
{
    cleanResource();
    if (m_cbFrame) {
        delete m_cbFrame;
        m_cbFrame = NULL;
    }

    int item = 0;
    Q_FOREACH(ImportData* s, m_lDataList) {
        if (s) {
            delete s;
        }
        item++;
    }

    m_lDataList.clear();
}

void CIedShowWidget::cleanResource()
{
    m_scdFileName = "";

    for(int i = 0; i < m_vectorIEDInfo.size(); i++) {
        delete m_vectorIEDInfo[i];
    }
    m_vectorIEDInfo.clear();
}

void CIedShowWidget::createSelectedIEDWidget()
{
    const QString captionName[CH_MAX_COLUMN] = {
        CIedShowWidget::tr("Num"),
        CIedShowWidget::tr("APPID"),
        CIedShowWidget::tr("MAC"),
        CIedShowWidget::tr("Type")
    };

    m_pSelectedIedWidget = new QTableWidget(this);
    m_pSelectedIedWidget->verticalHeader()->hide();
    m_pSelectedIedWidget->setMinimumWidth(340);
    m_pSelectedIedWidget->setMaximumWidth(400);
    m_pSelectedIedWidget->setColumnCount(CH_MAX_COLUMN);

    //m_pSelectedIedWidget->verticalHeader()->hide();

    for(int i = 0; i < CH_MAX_COLUMN; i++) {
        QTableWidgetItem *headerItem = new QTableWidgetItem(captionName[i]);
        m_pSelectedIedWidget->setHorizontalHeaderItem(i, headerItem);
        m_pSelectedIedWidget->setColumnWidth(i, CH_COL_WIDTH[i]);
    }

    m_pSelectedIedWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void CIedShowWidget::addSelectedIED(const ImportData & import)
{
    //Add Row
    int row = m_pSelectedIedWidget->rowCount();
    m_pSelectedIedWidget->setRowCount(row+1);

    // Num
    m_pSelectedIedWidget->setItem(row,CH_NUM,new QTableWidgetItem(QString::number(row + 1)));

    // APPID
    QString strTmp = QString("%1").arg(import.appId, 4, 16, QChar('0'));
    m_pSelectedIedWidget->setItem(row, CH_APPID, new QTableWidgetItem(strTmp.toUpper()));

    //MAC Addr
    strTmp = QString("%1-%2-%3-%4-%5-%6") 
        .arg(import.frame[0], 2, 16, QChar('0'))
        .arg(import.frame[1], 2, 16, QChar('0'))
        .arg(import.frame[2], 2, 16, QChar('0'))
        .arg(import.frame[3], 2, 16, QChar('0'))
        .arg(import.frame[4], 2, 16, QChar('0'))
        .arg(import.frame[5], 2, 16, QChar('0'));
    m_pSelectedIedWidget->setItem(row, CH_MAC, new QTableWidgetItem(strTmp.toUpper()));

    // Type
    strTmp = import.frameStyle;
    m_pSelectedIedWidget->setItem(row, CH_TYPE, new QTableWidgetItem(strTmp));
}

void CIedShowWidget::delSelectedIED()
{
    m_pSelectedIedWidget->setRowCount(0);
    m_pSelectedIedWidget->clearContents(); // 删除原有的内容
    for (int i = 0; i < m_lDataList.size();i++)
    {
        addSelectedIED(*(m_lDataList[i]));
    }
}

int CIedShowWidget::findIED(int appid)
{
    int item = 0;
    Q_FOREACH(ImportData *s,m_lDataList)
    {
        if (s->appId == appid)
        {
            return item;
        }
        item ++;
    }

    return -1;
}

bool CIedShowWidget::delIED(int index)
{
    if (-1 == index || index > m_lDataList.size())
    {
        return false;
    }

    delete m_lDataList.takeAt(index);

    return true;
}

void CIedShowWidget::createBtns()
{
    m_pSureBtn = new QPushButton(tr("Sure"));
    m_pCancelBtn = new QPushButton(tr("Cancel"));

    connect(m_pCancelBtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(m_pSureBtn,SIGNAL(clicked()),this,SLOT(onSure()));
}

static inline bool ledNameLessThan(const IED_INFO* ied1, const IED_INFO * ied2)
{
    return ied1->name.toLower() < ied2->name.toLower();
}

bool CIedShowWidget::parseFile(const QString & fileName)
{
    bool rc = false;
    m_scdFileName = fileName;
    CScdAnalysis* analysis = CScdAnalysis::getInstance();

    if (!analysis->parseFile(m_scdFileName)) {
        goto exitCode;
    }
    
    int iedNum = analysis->smvIedNum();
    for(int i = 0; i < iedNum; i++) {
        if (analysis->smvIedInfo(i)->iedName.length())
            addIed(analysis->smvIedInfo(i));
    }

    iedNum = analysis->gseIedNum();
    for(int i = 0; i < iedNum; i++) {
        if (analysis->gseIedInfo(i)->iedName.length())
            addIed(analysis->gseIedInfo(i));
    }

    qSort(m_vectorIEDInfo.begin(), m_vectorIEDInfo.end(), ledNameLessThan);
    int item = 0;
    Q_FOREACH(IED_INFO* ied, m_vectorIEDInfo) {
        ied->item = item++;
    }

    if (item == 0) {
        goto exitCode;
    }

    _setupUI();

    rc = true;
exitCode:
    if (!rc) {
        cleanResource();
    }

    return rc;
}


void CIedShowWidget::updateIedInfo(const IED_INFO* iedInfo)
{
    if (iedInfo == NULL) {
        m_iedLabel->setText("No IED");
    }
    else {
        m_iedLabel->setText(QString("%1-[%2]%3").arg(iedInfo->item+1).arg(iedInfo->name).arg(iedInfo->desc));
    }
    m_cbShowWidget->updateIedInfo(iedInfo);
}


void CIedShowWidget::cbSelect(const SelectedCBInfo &cbInfo, const quint8* data, quint32 size , bool flag)
{
    if (m_cbFrame) {
        delete m_cbFrame;
    }
    m_cbFrame = new quint8[size];
    memcpy(m_cbFrame, data, size);
    m_length = size;
    switch(cbInfo.cbType) {
    case CB_TX_SMV:
    case CB_RX_SMV:
        m_frameType = "SMV92";
        break;
    case CB_TX_GOOSE:
    case CB_RX_GOOSE:
        m_frameType = "GOOSE";
        break;
    default:
        m_frameType = "UNKNOWN";
        break;
    }
    m_appid = cbInfo.cbAppid;

    if (flag)
    {
        ImportData * importDate = new ImportData();
        selectedFrame(*importDate);
        addSelectedIED(*importDate);
        m_lDataList.push_back(importDate);
    }
    else
    {
        delIED(findIED(m_appid));
        delSelectedIED();
    }
    
}

void CIedShowWidget::onSure()
{
    if (m_lDataList.size() <= 0)
    {
        QMessageBox::warning(this, tr("Error"), tr("Import fail"));
    }
    else
    {
        this->accept();
    }
}

bool CIedShowWidget::selectedFrame(ImportData &data)
{
    Q_ASSERT(m_cbFrame);

    if (!data.frame || MAX_FRAME_SIZE < m_length) {
        return false;
    }

    memcpy(data.frame, m_cbFrame, m_length);
    data.frameLength = m_length;
    data.iedName = m_curIed->name;
    data.appId = m_appid;
    data.frameStyle = m_frameType;
    return m_length;
}

bool CIedShowWidget::selectedFrame(QList<ImportData*> &dataList)
{
    dataList.clear();

    for (int i = 0;i < m_lDataList.size();i++)
    {
        ImportData *importData = new ImportData();
        importData->frameStyle = m_lDataList[i]->frameStyle;
        importData->appId = m_lDataList[i]->appId;
        importData->iedName = m_lDataList[i]->iedName;
        importData->frameLength = m_lDataList[i]->frameLength;
        memcpy(importData->frame, m_lDataList[i]->frame, m_lDataList[i]->frameLength);
        dataList.push_back(importData);
    }

    return true;
}

void CIedShowWidget::_setupUI()
{
    m_treeWidgetIED = createIEDTree();
    QTreeWidgetItem* topItem = m_treeWidgetIED->topLevelItem(0);
    topItem->setExpanded(true);
    topItem->child(0)->setExpanded(true);
    m_treeWidgetIED->setMinimumWidth(100);
    m_treeWidgetIED->setMaximumWidth(400);

    m_cbShowWidget = createCBTabWidget();
    m_cbShowWidget->setMinimumWidth(100);

    m_iedLabel = new QLabel("No IED", this);

    createSelectedIEDWidget();

    createBtns();

    _layout();

    connect(m_cbShowWidget, SIGNAL(cbSelected(const SelectedCBInfo &, const quint8*, quint32, bool)), this, SLOT(cbSelect(const SelectedCBInfo &, const quint8*, quint32, bool)));
    connect( m_treeWidgetIED, SIGNAL(itemSelectionChanged()), this, SLOT(iedTreeChanged()));
    m_treeWidgetIED->setFocus();
}

void CIedShowWidget::_layout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(m_treeWidgetIED);
    leftLayout->addWidget(new QLabel(tr("Selected IED:")));
    leftLayout->addWidget(m_pSelectedIedWidget);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_iedLabel);
    rightLayout->addWidget(m_cbShowWidget);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_pSureBtn);
    bottomLayout->addWidget(m_pCancelBtn);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addLayout(leftLayout);
    topLayout->addLayout(rightLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);

    mainLayout->setMargin(1);

    this->setLayout(mainLayout);
}

QTreeWidget* CIedShowWidget::createIEDTree()
{
    QTreeWidget*  treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderHidden(true);

    QDir dir(m_scdFileName);
    QTreeWidgetItem* fileItem = new QTreeWidgetItem(QStringList(dir.dirName()));
    QTreeWidgetItem* iedItem = new QTreeWidgetItem(fileItem, QStringList(tr("IED Device")));
    QList<QTreeWidgetItem *> iedDevs;
    Q_FOREACH(IED_INFO* ied, m_vectorIEDInfo) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(iedItem, QStringList(QString(tr("%1-[%2]%3")).arg(ied->item+1).arg(ied->name).arg(ied->desc)));
        newItem->setData(0, Qt::UserRole, QVariant(reinterpret_cast<quint32>(ied)));
        iedDevs.append(newItem);
    }
    treeWidget->insertTopLevelItem(0, fileItem);

    return treeWidget;
}


CCBTabWidget* CIedShowWidget::createCBTabWidget()
{
    CCBTabWidget* cbTabWidget = new CCBTabWidget(this);

    return cbTabWidget;
}


bool CIedShowWidget::addIed(const SCDSMV92ANALYSIS_IEDSMV_INFO* iedInfo)
{
    QString iedName = QString::fromStdWString(iedInfo->iedName);
    int item = findIedByName(iedName);
    if (item == -1) {
        IED_INFO* ied = new IED_INFO(iedName, QString::fromStdWString(iedInfo->iedDesc));
        ied->hasTxSmv = true;
        m_vectorIEDInfo.push_back(ied);
    }
    else {
        IED_INFO* ied = m_vectorIEDInfo[item];
        ied->hasTxSmv = true;
    }

    return true;
}

bool CIedShowWidget::addIed(const SCDGOOSEANALYSIS_IEDGOOSE_INFO* iedInfo)
{
    QString iedName = QString::fromStdWString(iedInfo->iedName);
    int item = findIedByName(iedName);
    if (item == -1) {
        IED_INFO* ied = new IED_INFO(iedName, QString::fromStdWString(iedInfo->iedDesc));
        ied->hasTxGoose = true;
        m_vectorIEDInfo.push_back(ied);
    }
    else {
        IED_INFO* ied = m_vectorIEDInfo[item];
        ied->hasTxGoose = true;
    }

    return true;
}

int CIedShowWidget::findIedByName(const QString & name)
{
    int size = m_vectorIEDInfo.size();
    for(int i = 0; i < size; i++) {
        if (name == m_vectorIEDInfo[i]->name) {
            return i;
        }
    }

    return -1;
}


void CIedShowWidget::iedTreeChanged()
{
    QTreeWidgetItem *item = m_treeWidgetIED->currentItem();
    QVariant var = item->data(0, Qt::UserRole);
    bool ok = false;
    quint32 tmp = var.toUInt(&ok);
    if (ok) {
        IED_INFO* ied = reinterpret_cast<IED_INFO*>(tmp);
        if (m_curIed != ied) {
            updateIedInfo( ied );
            m_curIed = ied;
        }
    }
    else {
        updateIedInfo(NULL);
        m_curIed = NULL;
    }
}