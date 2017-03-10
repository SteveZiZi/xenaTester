#include "TxCBShowWidget.h"

#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QBoxLayout>
#include <QtGui/QLabel>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

ITxCBShowWidget::ITxCBShowWidget(CB_TYPE type, QWidget *parent/* = 0*/, Qt::WFlags flags/* = 0*/)
: ICBShowBase(type, parent,flags)
, m_frameBuf(NULL)
{
    Q_ASSERT(type == CB_TX_SMV || type == CB_TX_GOOSE);
}

ITxCBShowWidget::~ITxCBShowWidget()
{
    if (m_frameBuf) {
        delete[] m_frameBuf;
        m_frameBuf = NULL;
    }
}



void ITxCBShowWidget::setupUI()
{
    m_captionLabel = new QLabel(this);
    if (m_cbType == CB_TX_SMV) {
        m_captionLabel->setText("SMV控制块列表");
    }
    else {
        m_captionLabel->setText("GOOSE控制块列表");
    }

    m_tableWidgetCB = createCBTableWidget();
    m_tableWidgetCB->setMinimumWidth(100);
    m_tableWidgetCB->setMinimumHeight(50);
    m_tableWidgetCB->setSelectionMode(QTableWidget::SingleSelection);

    m_tableWidgetChnl = createChnlTableWidget();
    m_tableWidgetChnl->setMinimumWidth(100);
    m_tableWidgetChnl->setMinimumHeight(50);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_captionLabel);
    mainLayout->addWidget(m_tableWidgetCB);
    mainLayout->addWidget(m_tableWidgetChnl);
    
    setLayout(mainLayout);

    //Add Menu
    m_selectionAction = new QAction(tr("导入"),this);
    connect(m_selectionAction,SIGNAL(triggered()),this,SLOT(exprotCB()));
    m_tableWidgetCB->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tableWidgetCB,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(onCustomContextMenuRequested(const QPoint&)));
}


void ITxCBShowWidget::exprotCB()
{
    SelectedCBInfo cbInfo;
    int len = exportFrame(cbInfo.cbAppid, cbInfo.cbMac);
    if (len > 0 && m_frameBuf) {
        cbInfo.iedName = m_iedInfo.name;
        cbInfo.cbType = m_cbType;
        emit cbSelected(cbInfo, m_frameBuf, len);
    }
    else {
        QMessageBox::warning(this, tr("错误"), tr("导入控制块失败"));
    }
}


void ITxCBShowWidget::onCustomContextMenuRequested(const QPoint &point)
{
    qDebug() << "onCustomContextMenuRequested";

    QTableWidgetItem* item = m_tableWidgetCB->itemAt(point);
    if (!item) {
        return;
    }

    QMenu contextMenu;
    contextMenu.addAction(m_selectionAction);
    contextMenu.exec(QCursor::pos());  
}