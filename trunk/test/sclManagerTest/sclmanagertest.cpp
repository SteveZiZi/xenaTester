#include "sclmanagertest.h"
#include "XenaManager/src/dialog/ImportData/utilTools.h"

#include <QDebug>
#include <QBoxLayout>
#include <QtGui>

#include "XenaManager/src/common_ctrl/ethSegEdit/ethSegEditDelegate.h"
#include "XenaManager/src/licencesMgr/licencesMgr.h"


sclManagerTest::sclManagerTest(QWidget *parent, Qt::WFlags flags)
    : QDialog(parent, flags)
{
    QPushButton* ptn1 = new QPushButton(tr("导入SCD"), this);
    QPushButton* ptn2 = new QPushButton(tr("导入PCAP"), this);
    QPushButton* ptn3 = new QPushButton(tr("登录"), this);
    QPushButton* ptn4 = new QPushButton(tr("模板测试"), this);
    QPushButton* ptn5 = new QPushButton(tr("跳变域"), this);
    QPushButton* ptn6 = new QPushButton(tr("文本框"), this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(ptn1);
    layout->addWidget(ptn2);
    layout->addWidget(ptn3);
    layout->addWidget(ptn4);
    layout->addWidget(ptn5);
    layout->addWidget(ptn6);
    this->setLayout(layout);

    connect(ptn1, SIGNAL(clicked()), this, SLOT(importScdFile()));
    connect(ptn2, SIGNAL(clicked()), this, SLOT(importPcapFile()));
    connect(ptn3, SIGNAL(clicked()), this, SLOT(login()));
    connect(ptn4, SIGNAL(clicked()), this, SLOT(testTemplate()));
    connect(ptn5, SIGNAL(clicked()), this, SLOT(modifierTest()));
    connect(ptn6, SIGNAL(clicked()), this, SLOT(showFrame()));
}

sclManagerTest::~sclManagerTest()
{

}


void sclManagerTest::importScdFile()
{
    QString scdFileName = QFileDialog::getOpenFileName(0, 
                                                       tr("打开SCD文件"),
                                                       QString(),
                                                       tr("SCL (*.scd *.icd *.cid);; 所有文件(*.*)"));
    qDebug() << scdFileName;
    if (!scdFileName.isEmpty()) {
        _importFile(IMPORT_SCD_FILE, scdFileName);
    }
}

void sclManagerTest::importPcapFile()
{
    QString pcapFileName = QFileDialog::getOpenFileName(0, 
                                                       tr("打开PCAP文件"),
                                                       QString(),
                                                       tr("PCAP (*.pcap);; 所有文件(*.*)"));
    qDebug() << pcapFileName;
    if (!pcapFileName.isEmpty()) {
        _importFile(IMPORT_PCAP_FILE, pcapFileName);
    }
}

void sclManagerTest::_importFile(IMPORT_FILE_STYLE fileStyle, const QString& fileName)
{
    ImportDataDialog* importDlg = CreateImportDataDlg(fileStyle, this);
    if (importDlg->parseFile(fileName)) {
        bool rc = importDlg->exec();
        if (rc) {
            QDialog * modelTestDlg = new QDialog(this);
            QTreeView *treeView = new QTreeView(modelTestDlg);
            treeView->setItemDelegate(new CEthSegEditDelegate(treeView));

            QVBoxLayout *mainLayout = new QVBoxLayout();
            mainLayout->addWidget(treeView);
            mainLayout->setMargin(1);
            modelTestDlg->setLayout(mainLayout);
            modelTestDlg->resize(610, 500);

            QStandardItemModel* itemModel = new QStandardItemModel(modelTestDlg);
            treeView->setModel(itemModel);
            rc = importDlg->selectedFrame(m_importData);
            ethFrameToItemModel(m_importData.frame, m_importData.frameLength, itemModel);
            treeView->setColumnWidth(0, 200);
            treeView->setColumnWidth(1, 400);
            treeView->expandAll();

            QString title(m_importData.frameStyle);
            title.append(QString("-%1").arg(m_importData.appId, 4, 16, QChar('0')).toUpper());
            if (fileStyle == IMPORT_SCD_FILE) {
                title.append('-');
                title.append(m_importData.iedName);
            }
            modelTestDlg->setWindowTitle(title);

            connect(itemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(frameModified(QStandardItem *)));
            modelTestDlg->exec();
            disconnect(itemModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(frameModified(QStandardItem *)));
            modelTestDlg->deleteLater();
        }
    }
    ReleaseImportDataDlg(importDlg);
}

void sclManagerTest::frameModified(QStandardItem *item)
{
    eth_segment_info info;
    if (ethGetSegmentInfo(item, info)) {
        Q_ASSERT(info.pos+info.length <= m_importData.frameLength);
        QStringList dataList = item->text().split(' ');
        quint8* data = m_importData.frame+info.pos;
        Q_FOREACH(QString str, dataList)
            *data++ = str.toUInt(0, 16);
    }
}

#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/Communicate/deviceImpl.h"

#define DEVICE_IMPL 0
#define LOCAL_XENA  0
void sclManagerTest::login()
{

#if DEVICE_IMPL
    //CDeviceImpl device;
    CDeviceImpl* device = new CDeviceImpl();

#if LOCAL_XENA
    qDebug() << device->login(QString("192.168.0.202"), 22611, QString("XuanTong"), QString("xena"));
#else
    qDebug() << device.login(QString("131.164.227.250"), 22611, QString("XuanTong"), QString("xena"));
#endif

//     if (device.isLogin())
//     {
//         QString text;
//         qDebug() << device.chassisName(text);
//         qDebug() << text;
// 
//         qDebug() << device.chassisSerialNo(text);
//         qDebug() << text;
// 
//         qDebug() << device.chassisReservedOwner(text);
//         qDebug() << text;
// 
//         //qDebug() << device.reservedChassis();
// #if 1
//         int moduleCnt =  device.moduleCount();    
//         for(int i = 0; i < moduleCnt; i++) {
//             qDebug() << device.moduleName(i, text);
//         }
// #else
//         QStringList moduleNameList;
//         qDebug() << device.allModuleName(moduleNameList);
//         Q_FOREACH(const QString& str, moduleNameList)
//             qDebug() << str;
// #endif
//     }

#else

    QDateTime t1 = QDateTime::currentDateTime();
    CXenaTester tester;

#if LOCAL_XENA
    tester.login(QString("192.168.0.202"), 22611, QString("XTOEE"), QString("xena"));
#else
    tester.login(QString("131.164.227.250"), 22611, QString("XTOEE"), QString("xena"));
#endif

    if (tester.isLogin())
    {
        tester.queryChassisInfo();

        CChassis* chassis = tester.chassis(); 
        qDebug() << chassis->chassisName() << chassis->chassisSerialNo();

        chassis->dumpMsg();

        tester.logout();

        QDateTime t2 = QDateTime::currentDateTime();
        QMessageBox::information(this, QString("TM"), QString::number(t1.msecsTo(t2)));
    }
#endif
}

#include "XenaManager/src/template/testTemplate.h"
#define TEMPLATE_FILE "C:\\Users\\Administrator\\Desktop\\port.xml"
void sclManagerTest::testTemplate()
{
    CPort* port = new CPort();

    ImportData data;
    data.appId = 0x2001;
    data.frameStyle = "SMV92";
    data.iedName = "UNKNOWN";
    data.frameLength = 100;
    for(int i = 0; i < 100; i++) {
        data.frame[i] = i*2;
    }

    for(int i = 0; i < 1; i++) {
        CStream* stream = new CStream();
        port->appendStream(stream);
        stream->setData(data);
        stream->setTxFramesPerSecond(4000);
    }

    CTestTemplate testTemplate;
    testTemplate.saveTemplate(port, TEMPLATE_FILE);
    delete port;


    CTestTemplate importTemplate;
    CTestTemplate::Type type = importTemplate.templateType(TEMPLATE_FILE);
    if (type == CTestTemplate::TYPE_PORT) {
        CPort port;
        importTemplate.loadTemplate(TEMPLATE_FILE, &port);
        port.dumpMsg();
    }
}

#include "XenaManager/src/dialog/modifierDialog/modifierDialog.h"
void sclManagerTest::modifierTest()
{
    CModifierDialog *dlg = new CModifierDialog(eth_segment_info(10, 15), this);
    dlg->show();
}

#include "XenaManager/src/common_ctrl/frameShowWidget/frameShowWidget.h"
void sclManagerTest::showFrame()
{
    quint8 *data = new quint8[254];
    for(int i = 0; i < 254; i++) {
        data[i] = i*i + 3*i +7;
    }
    QDialog *dlg = new QDialog(this);

    CFrameShowWidget *frameWidget = new CFrameShowWidget(data, 254, dlg);

#if 1
    frameWidget->appendHighLight(0, 2);
    frameWidget->appendHighLight(7, 2);
    frameWidget->appendHighLight(15, 2);

    frameWidget->appendHighLight(24, 2);
    frameWidget->appendHighLight(32, 2);

    frameWidget->appendHighLight(63, 2);

    frameWidget->appendHighLight(128, 2);

    frameWidget->appendHighLight(252, 2);
#else
    frameWidget->appendHighLight(0, 1);
    frameWidget->appendHighLight(7, 1);
    frameWidget->appendHighLight(8, 1);
    frameWidget->appendHighLight(15, 1);
    frameWidget->appendHighLight(16, 1);

    frameWidget->appendHighLight(23, 1);
    frameWidget->appendHighLight(24, 1);
    frameWidget->appendHighLight(31, 1);
    frameWidget->appendHighLight(32, 1);

    frameWidget->appendHighLight(63, 1);
    frameWidget->appendHighLight(64, 1);

    frameWidget->appendHighLight(127, 1);
    frameWidget->appendHighLight(128, 1);

    frameWidget->appendHighLight(253, 1);

#endif

    frameWidget->setSelected(10, 16);
    frameWidget->appendHighLight(10, 2);
    frameWidget->clearSelected();
    quint8 testData[2] = {0xFF, 0xFF};
    frameWidget->updateFrame(252, 2, testData);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(frameWidget);
    dlg->setLayout(layout);
    dlg->setMinimumWidth(350);
    dlg->setMinimumHeight(400);
    dlg->show();

    delete[] data;
}