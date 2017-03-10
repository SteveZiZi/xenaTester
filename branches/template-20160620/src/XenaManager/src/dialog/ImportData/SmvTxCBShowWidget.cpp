#include "SmvTxCBShowWidget.h"

#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QScrollBar>
#include <QtGui/QHeaderView>
#include <QBoxLayout>
#include <QtGui/QLabel>

#include "XenaManager/src/service/fileAnalysis/scdAnalysis/scdAnalysis.h"


#define TABLE_HEADER_WIDTH   30


const int CB_COL_WIDTH[CSmvTxCBShowWidget::MAX_CB_COL_NUM] = {
    50,     //APPID
    120,    //MAC ADDR
    60,     //CHNL NUM
    80,     //DataSet Desc
    180,    //DataSet
    160,    //goID
    60,
    80,
    80
};

const int CHNL_COL_WIDTH[CSmvTxCBShowWidget::MAX_CHNL_COL_NUM] = {
    100, 100, 100, 100, 220
};



CSmvTxCBShowWidget::CSmvTxCBShowWidget(QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
: ITxCBShowWidget(CB_TX_SMV, parent,flags)
{
    m_analysis = CScdAnalysis::getInstance();
}

CSmvTxCBShowWidget::~CSmvTxCBShowWidget()
{
}


//override from ICBShowBase
void CSmvTxCBShowWidget::updateShow()
{
    m_tableWidgetCB->clearContents();
    m_tableWidgetCB->setRowCount(0);

    if (!m_iedInfo.hasTxSmv) {
        m_tableWidgetChnl->clearContents();
        m_tableWidgetChnl->setRowCount(0);
        return ;
    }

    int iedNum = m_analysis->smvIedNum();
    for(int i = 0; i < iedNum; i++) {
        if ( m_analysis->smvIedInfo(i)->iedName == m_iedInfo.name.toStdWString() )
            addSmvCB(*m_analysis->smvIedInfo(i));
    }
    m_tableWidgetCB->setCurrentCell(0, 0);
}


void CSmvTxCBShowWidget::cbChanged()
{
    int row = m_tableWidgetCB->currentRow();
    quint16 appId = m_tableWidgetCB->item(row, CB_COL_APPID)->text().toInt(0, 16);
    QString strMac = m_tableWidgetCB->item(row, CB_COL_MAC_ADDR)->text();

    const SCDSMV92ANALYSIS_IEDSMV_INFO *smvCB = m_analysis->smvIedInfo(strMac, appId);
    if (smvCB == NULL) {
        return ;
    }

    m_tableWidgetChnl->clearContents();
    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *channelInfoArr = new SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO[smvCB->channelNum];
    int channelActNum = m_analysis->smvChnlInfo(smvCB->iedName, smvCB->iedLdInst, smvCB->smvCbName,channelInfoArr,smvCB->channelNum);

    m_tableWidgetChnl->setRowCount(channelActNum);
    //QString chnlPath;
    for(int row = 0; row < channelActNum; row++) {
        m_tableWidgetChnl->setItem(row, CHNL_COL_ITEM, new QTableWidgetItem(QString::number(channelInfoArr[row].idxSeq)));
        m_tableWidgetChnl->setItem(row, CHNL_COL_DESC_LN, new QTableWidgetItem(QString::fromStdWString(channelInfoArr[row].lnDesc)));
        m_tableWidgetChnl->setItem(row, CHNL_COL_DESC_DO, new QTableWidgetItem(QString::fromStdWString(channelInfoArr[row].doDesc)));
        //m_tableWidgetChnl->setItem(row, CHNL_COL_DESC_DU, new QTableWidgetItem(QString::fromStdWString(channelInfoArr[row].dUVal)));
//         chnlPath = QString("%1/%2/%3%4%5.%6.%7")
//                             .arg(QString::fromStdWString(smvCB.iedName))
//                             .arg(QString::fromStdWString(channelInfoArr[row].ldInst))
//                             .arg(QString::fromStdWString(channelInfoArr[row].lnPrefix))
//                             .arg(QString::fromStdWString(channelInfoArr[row].lnClass))
//                             .arg(QString::fromStdWString(channelInfoArr[row].ldInst))
//                             .arg(QString::fromStdWString(channelInfoArr[row].doName))
//                             .arg(QString::fromStdWString(channelInfoArr[row].daName));
//         m_tableWidgetChnl->setItem(row, CHNL_COL_PATH, new QTableWidgetItem(chnlPath));

        for(int i = 0; i < MAX_CHNL_COL_NUM; i++) {
            if (m_tableWidgetChnl->item(row, i))
                m_tableWidgetChnl->item(row, i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        }
    }

    delete[] channelInfoArr;
}


void CSmvTxCBShowWidget::addSmvCB(const SCDSMV92ANALYSIS_IEDSMV_INFO& smvCB)
{
    //Add Row
    int row = m_tableWidgetCB->rowCount();
    m_tableWidgetCB->setRowCount(row+1);

    //APPID
    QString strTmp = QString("%1").arg(smvCB.smvAppID, 4, 16, QChar('0'));
    m_tableWidgetCB->setItem(row, CB_COL_APPID, new QTableWidgetItem(strTmp.toUpper()));

    //MAC Addr
    strTmp = QString("%1-%2-%3-%4-%5-%6") 
                    .arg(smvCB.smvMAC[0], 2, 16, QChar('0'))
                    .arg(smvCB.smvMAC[1], 2, 16, QChar('0'))
                    .arg(smvCB.smvMAC[2], 2, 16, QChar('0'))
                    .arg(smvCB.smvMAC[3], 2, 16, QChar('0'))
                    .arg(smvCB.smvMAC[4], 2, 16, QChar('0'))
                    .arg(smvCB.smvMAC[5], 2, 16, QChar('0'));
    m_tableWidgetCB->setItem(row, CB_COL_MAC_ADDR, new QTableWidgetItem(strTmp.toUpper()));
    //  通道数目
    m_tableWidgetCB->setItem(row, CB_COL_CHNL_NUM, new QTableWidgetItem(QString::number(smvCB.channelNum)));
    //数据集描述
    m_tableWidgetCB->setItem(row, CB_COL_DATA_DESC, new QTableWidgetItem(QString::fromStdWString(smvCB.dataSetDesc)));
    //数据集地址
    strTmp = QString("%1%2$%3")
                    .arg(QString::fromStdWString(smvCB.iedName))
                    .arg(QString::fromStdWString(smvCB.iedLdInst))
                    .arg(QString::fromStdWString(smvCB.dataSetName));
    m_tableWidgetCB->setItem(row, CB_COL_DATASET, new QTableWidgetItem(strTmp));
    //svID
    m_tableWidgetCB->setItem(row, CB_COL_SVID, new QTableWidgetItem(QString::fromStdWString(smvCB.smvCbSmvID)));
    //VLAN-ID
    m_tableWidgetCB->setItem(row, CB_COL_VLAN_ID, new QTableWidgetItem(QString::number(smvCB.smvVlanID)));
    //VLAN-Prio
    m_tableWidgetCB->setItem(row, CB_COL_VLAN_PRIO, new QTableWidgetItem(QString::number(smvCB.smvVlanPri)));
    //confRev
    m_tableWidgetCB->setItem(row, CB_COL_CONF_REF, new QTableWidgetItem(QString::number(smvCB.smvCbConfRev)));

    for(int i = 0; i < MAX_CB_COL_NUM; i++)
        m_tableWidgetCB->item(row, i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}

//override from ITxCBShowWidget
QTableWidget* CSmvTxCBShowWidget::createCBTableWidget()
{
    const QString captionName[MAX_CB_COL_NUM] = {
        CSmvTxCBShowWidget::tr("APPID"),
        CSmvTxCBShowWidget::tr("MAC地址"),
        CSmvTxCBShowWidget::tr("通道数目"),
        CSmvTxCBShowWidget::tr("数据集描述"),
        CSmvTxCBShowWidget::tr("数据集地址"),
        CSmvTxCBShowWidget::tr("SvID"),
        CSmvTxCBShowWidget::tr("VLAN-ID"),
        CSmvTxCBShowWidget::tr("VLAN-优先级"),
        CSmvTxCBShowWidget::tr("配置版本号")
    };

    QTableWidget* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(MAX_CB_COL_NUM);

    tableWidget->verticalHeader()->hide();

    for(int i = 0; i < MAX_CB_COL_NUM; i++) {
        QTableWidgetItem *headerItem = new QTableWidgetItem(captionName[i]);
        tableWidget->setHorizontalHeaderItem(i, headerItem);
        tableWidget->setColumnWidth(i, CB_COL_WIDTH[i]);
    }

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(cbChanged()));

    return tableWidget;
}

QTableWidget* CSmvTxCBShowWidget::createChnlTableWidget()
{
    const QString captionName[MAX_CHNL_COL_NUM] = {
        CSmvTxCBShowWidget::tr("序号"),
        CSmvTxCBShowWidget::tr("LN描述"),
        CSmvTxCBShowWidget::tr("DO描述"),
        CSmvTxCBShowWidget::tr("DU描述"),
        CSmvTxCBShowWidget::tr("路径"),
    };

    QTableWidget* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(MAX_CHNL_COL_NUM);

    tableWidget->verticalHeader()->hide();

    QHeaderView* hHeader = new QHeaderView(Qt::Horizontal, tableWidget);
    hHeader->setObjectName(QString("chnlTableWidget"));
    tableWidget->setHorizontalHeader(hHeader);

    for(int i = 0; i < MAX_CHNL_COL_NUM; i++) {
        QTableWidgetItem *headerItem = new QTableWidgetItem(captionName[i]);
        tableWidget->setHorizontalHeaderItem(i, headerItem);
        tableWidget->setColumnWidth(i, CHNL_COL_WIDTH[i]);
    }

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    return tableWidget;
}

int CSmvTxCBShowWidget::exportFrame(quint16 &appid, QString& mac)
{
    int row = m_tableWidgetCB->currentRow();
    if (row < 0) {
        return 0;
    }
    appid = m_tableWidgetCB->item(row, CB_COL_APPID)->text().toInt(0, 16);
    mac = m_tableWidgetCB->item(row, CB_COL_MAC_ADDR)->text();

    if (!m_frameBuf) {
        m_frameBuf = new quint8[1536];
    }
    
    return m_analysis->smvGenerateFrame(mac, appid, m_frameBuf, 1536);
}