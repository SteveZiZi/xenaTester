#include "GooseTxCBShowWidget.h"

#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QScrollBar>
#include <QtGui/QHeaderView>
#include <QBoxLayout>
#include <QtGui/QLabel>

#include <QDebug>

#include "XenaManager/src/service/fileAnalysis/scdAnalysis/scdAnalysis.h"

#define TABLE_HEADER_WIDTH   30


const int CB_COL_WIDTH[CGooseTxCBShowWidget::MAX_CB_COL_NUM] = {
    40,     // Check
    50,     //APPID
    120,    //MAC ADDR
    60,     //CHNL NUM
    80,     //DataSet Desc
    180,    //DataSet
    160,    //goID
    180,     //gocbRef
    60,     //Vlan ID
    80,     //Vlan Prio
    80      //config Ref
};

const int CHNL_COL_WIDTH[CGooseTxCBShowWidget::MAX_CHNL_COL_NUM] = {
    50, 100, 100, 100, 100, 220
};


CGooseTxCBShowWidget::CGooseTxCBShowWidget(QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
: ITxCBShowWidget(CB_TX_GOOSE, parent,flags)
{
    m_analysis = CScdAnalysis::getInstance();
}

CGooseTxCBShowWidget::~CGooseTxCBShowWidget()
{
}


//override from ICBShowBase
void CGooseTxCBShowWidget::updateShow()
{
    disLoaclConnect();
    m_tableWidgetCB->clearContents();
    m_tableWidgetCB->setRowCount(0);

    if (!m_iedInfo.hasTxGoose) {
        m_tableWidgetChnl->clearContents();
        m_tableWidgetChnl->setRowCount(0);
        return ;
    }

    int iedNum = m_analysis->gseIedNum();
    for(int i = 0; i < iedNum; i++) {
        if ( m_analysis->gseIedInfo(i)->iedName == m_iedInfo.name.toStdWString() )
            addGseCB(*m_analysis->gseIedInfo(i));
    }
    m_tableWidgetCB->setCurrentCell(0, 0);
    localConnect();
}


void CGooseTxCBShowWidget::cbChanged()
{
    int row = m_tableWidgetCB->currentRow();
    quint16 appId = m_tableWidgetCB->item(row, CB_COL_APPID)->text().toInt(0, 16);
    QString strMac = m_tableWidgetCB->item(row, CB_COL_MAC_ADDR)->text();

    const SCDGOOSEANALYSIS_IEDGOOSE_INFO *gseCB = m_analysis->gseIedInfo(strMac, appId);
    if (gseCB == NULL) {
        return ;
    }

    m_tableWidgetChnl->clearContents();
    SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *channelInfoArr = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[gseCB->channelNum];
    int channelActNum = m_analysis->gseChnlInfo(gseCB->iedName, gseCB->iedLdInst, gseCB->cbName,channelInfoArr,gseCB->channelNum);

    m_tableWidgetChnl->setRowCount(channelActNum);
    QString chnlPath;
    for(int row = 0; row < channelActNum; row++) {
        m_tableWidgetChnl->setItem(row, CHNL_COL_ITEM, new QTableWidgetItem(QString::number(channelInfoArr[row].idxSeq)));
        m_tableWidgetChnl->setItem(row, CHNL_COL_TYPE, new QTableWidgetItem(QString::fromStdWString(channelInfoArr[row].daBType)));
        m_tableWidgetChnl->setItem(row, CHNL_COL_DESC_LN, new QTableWidgetItem(QString::fromStdWString(channelInfoArr[row].lnDesc)));
        m_tableWidgetChnl->setItem(row, CHNL_COL_DESC_DO, new QTableWidgetItem(QString::fromStdWString(channelInfoArr[row].doDesc)));
        m_tableWidgetChnl->setItem(row, CHNL_COL_DESC_DU, new QTableWidgetItem(QString::fromStdWString(channelInfoArr[row].dUVal)));
        chnlPath = QString("%1/%2/%3%4%5.%6.%7")
                            .arg(QString::fromStdWString(gseCB->iedName))
                            .arg(QString::fromStdWString(channelInfoArr[row].ldInst))
                            .arg(QString::fromStdWString(channelInfoArr[row].lnPrefix))
                            .arg(QString::fromStdWString(channelInfoArr[row].lnClass))
                            .arg(QString::fromStdWString(channelInfoArr[row].ldInst))
                            .arg(QString::fromStdWString(channelInfoArr[row].doName))
                            .arg(QString::fromStdWString(channelInfoArr[row].daName));
        m_tableWidgetChnl->setItem(row, CHNL_COL_PATH, new QTableWidgetItem(chnlPath));

        for(int i = 0; i < MAX_CHNL_COL_NUM; i++)
            m_tableWidgetChnl->item(row, i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }

    delete[] channelInfoArr;
}

void CGooseTxCBShowWidget::addGseCB(const SCDGOOSEANALYSIS_IEDGOOSE_INFO& gseCB)
{
    //Add Row
    int row = m_tableWidgetCB->rowCount();
    m_tableWidgetCB->setRowCount(row+1);

    // CheckBox
    QTableWidgetItem *checkBox = new QTableWidgetItem(QString::number(row + 1));

    bool checked = false;
    for (int i = 0; i < m_checkedReport.size();i++)
    {
        if (m_checkedReport[i] == gseCB.gooseAppID)
        {
            checked = true;
            break;
        }
    }
    if (checked)
    {
        checkBox->setCheckState(Qt::Checked);
    }
    else
    {
        checkBox->setCheckState(Qt::Unchecked);
    }

    m_tableWidgetCB->setItem(row,CB_CHECKED,checkBox);

    //APPID
    QString strTmp = QString("%1").arg(gseCB.gooseAppID, 4, 16, QChar('0'));
    m_tableWidgetCB->setItem(row, CB_COL_APPID, new QTableWidgetItem(strTmp.toUpper()));

    //MAC Addr
    strTmp = QString("%1-%2-%3-%4-%5-%6") 
                    .arg(gseCB.gooseMAC[0], 2, 16, QChar('0'))
                    .arg(gseCB.gooseMAC[1], 2, 16, QChar('0'))
                    .arg(gseCB.gooseMAC[2], 2, 16, QChar('0'))
                    .arg(gseCB.gooseMAC[3], 2, 16, QChar('0'))
                    .arg(gseCB.gooseMAC[4], 2, 16, QChar('0'))
                    .arg(gseCB.gooseMAC[5], 2, 16, QChar('0'));
    m_tableWidgetCB->setItem(row, CB_COL_MAC_ADDR, new QTableWidgetItem(strTmp.toUpper()));
    //  通道数目
    m_tableWidgetCB->setItem(row, CB_COL_CHNL_NUM, new QTableWidgetItem(QString::number(gseCB.channelNum)));

    //数据集描述
    m_tableWidgetCB->setItem(row, CB_COL_DATA_DESC, new QTableWidgetItem(QString::fromStdWString(gseCB.dataSetDesc)));
    //数据集地址
    strTmp = QString("%1%2/LLN0$%3")
                    .arg(QString::fromStdWString(gseCB.iedName))
                    .arg(QString::fromStdWString(gseCB.iedLdInst))
                    .arg(QString::fromStdWString(gseCB.dataSet));
    m_tableWidgetCB->setItem(row, CB_COL_DATASET, new QTableWidgetItem(strTmp));
    //goID
    m_tableWidgetCB->setItem(row, CB_COL_GOID, new QTableWidgetItem(QString::fromStdWString(gseCB.gooseID)));
    //gocbRef
    strTmp = QString("%1%2/LLN0$GO$%3")
                    .arg(QString::fromStdWString(gseCB.iedName))
                    .arg(QString::fromStdWString(gseCB.iedLdInst))
                    .arg(QString::fromStdWString(gseCB.cbName));
    m_tableWidgetCB->setItem(row, CB_COL_GOCB_REF, new QTableWidgetItem(strTmp));
    //VLAN-ID
    m_tableWidgetCB->setItem(row, CB_COL_VLAN_ID, new QTableWidgetItem(QString::number(gseCB.gooseVlanID)));
    //VLAN-Prio
    m_tableWidgetCB->setItem(row, CB_COL_VLAN_PRIO, new QTableWidgetItem(QString::number(gseCB.gooseVlanPri)));
    //confRev
    m_tableWidgetCB->setItem(row, CB_COL_CONF_REF, new QTableWidgetItem(QString::number(gseCB.confRev)));

    for(int i = 1; i < MAX_CB_COL_NUM; i++)
        m_tableWidgetCB->item(row, i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}


//override from ITxCBShowWidget
QTableWidget* CGooseTxCBShowWidget::createCBTableWidget()
{
    const QString captionName[MAX_CB_COL_NUM] = {
        CGooseTxCBShowWidget::tr("Num"),
        CGooseTxCBShowWidget::tr("APPID"),
        CGooseTxCBShowWidget::tr("MAC"),
        CGooseTxCBShowWidget::tr("ChannelNum"),
        CGooseTxCBShowWidget::tr("DataDescribe"),
        CGooseTxCBShowWidget::tr("DataAddress"),
        CGooseTxCBShowWidget::tr("goID"),
        CGooseTxCBShowWidget::tr("gocbRef"),
        CGooseTxCBShowWidget::tr("VLAN-ID"),
        CGooseTxCBShowWidget::tr("VLAN-Priority"),
        CGooseTxCBShowWidget::tr("Version")
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

QTableWidget* CGooseTxCBShowWidget::createChnlTableWidget()
{
    const QString captionName[MAX_CHNL_COL_NUM] = {
        CGooseTxCBShowWidget::tr("Num"),
        CGooseTxCBShowWidget::tr("Type"),
        CGooseTxCBShowWidget::tr("LN Description"),
        CGooseTxCBShowWidget::tr("DO Description"),
        CGooseTxCBShowWidget::tr("DU Description"),
        CGooseTxCBShowWidget::tr("Path"),
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

int CGooseTxCBShowWidget::exportFrame(quint16 &appid, QString& mac)
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

    return m_analysis->gseGenerateFrame(mac, appid, m_frameBuf, 1536);
}
