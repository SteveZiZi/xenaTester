#include "xenamanager.h"
#include "XenaManager/src/Login/logindialog.h"
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/chassisconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/moduleconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/portconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/streamconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include <QtGui>

#if (defined _DEBUG) && (defined WIN32)
#include "vld.h"
#endif

XenaManager::XenaManager(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
	initXenaManager();
	readSettings();

	connect(m_pLoginDialog,SIGNAL(loginResult(bool)),this,SLOT(onLoginResult(bool)));
	connect(m_pChassisViewWidget,SIGNAL(updateConfigWidget(S_CURRENT_INDEX)),this,SLOT(onUpdateConfigWidget(S_CURRENT_INDEX)));    // 点击左侧treeView更新右侧配置界面
}

XenaManager::~XenaManager()
{
    delete m_pXenaTester;
}

void XenaManager::onLogin()
{
	m_pLoginDialog->show();
	m_pStatusBarLabel->setText(tr("登陆中..."));
}

void XenaManager::onImport()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("导入配置文件"),".",
		tr("XenaManager files(*.xml)"));
	if(!fileName.isEmpty())
	{
		/*loadFile(fileName);*/
	}
}

void XenaManager::onExport()
{

}

void XenaManager::onAbout()
{
	QMessageBox aboutMessageBox(QMessageBox::Information,tr("关于"),tr("<h2>XenaManger--炫通电气测试版本，请勿外传，谢谢。</h2>"));
	aboutMessageBox.setStandardButtons(QMessageBox::Ok);
	aboutMessageBox.setButtonText(QMessageBox::Ok,tr("确定"));
	aboutMessageBox.exec();
}

void XenaManager::onLoginResult(bool result)
{
	if (result)
	{
		m_pLoginDialog->close();
		m_pConnectAction->setEnabled(false);
		m_pChassisViewWidget->initTreeViewWidgetData();					// 将机箱信息显示出来
		m_pChassisViewWidget->setVisible(true);
		m_pCurrentBaseConfigWidget->setVisible(true);
        m_pStatusBarLabel->setText("");
	}
	else
	{
		QString tip = tr("登陆失败!");
		qDebug() << tip;
		QToolTip::showText(QCursor::pos(),tip);
		m_pConnectAction->setEnabled(true);
	}
}

void XenaManager::onUpdateConfigWidget(S_CURRENT_INDEX currenIndex)
{
	Q_ASSERT(currenIndex.currentType < MAX_SELECTION);
	if (m_pCurrentBaseConfigWidget != m_pBaseConfigWidget[currenIndex.currentType])
	{
		m_pCurrentBaseConfigWidget->hide();
		m_pCurrentBaseConfigWidget = m_pBaseConfigWidget[currenIndex.currentType];
		m_pCurrentBaseConfigWidget->setCurrentIndex(currenIndex.currentModuleIndex,currenIndex.currentPortIndex,currenIndex.currentStreamIndex);
		m_pCurrentBaseConfigWidget->updateConfigWidget();
		m_pCurrentBaseConfigWidget->show();
	}
	else
	{
		m_pCurrentBaseConfigWidget->setCurrentIndex(currenIndex.currentModuleIndex,currenIndex.currentPortIndex,currenIndex.currentStreamIndex);
		m_pCurrentBaseConfigWidget->updateConfigWidget();;
	}
}

void XenaManager::closeEvent(QCloseEvent *event)
{
    if (!isNoStream())
    {
        QMessageBox::warning(0,tr("警告"),tr("请清空所有端口！"),QString(tr("确定")));
        event->ignore();
    }
    else
    {
        writeSettings();
        event->accept();
    }
}

void XenaManager::readSettings()
{
	QSettings settings("Software Inc.","XenaManager");

	restoreGeometry(settings.value("geometry").toByteArray());
}

void XenaManager::writeSettings()
{
	QSettings settings("Software Inc.","XenaManager");
	settings.setValue("geometry",saveGeometry());
}

void XenaManager::createActions()
{
	m_pConnectAction  = new QAction(tr("登陆"),this);
	m_pConnectAction->setIcon(QIcon(":/images/connect.png"));
	m_pConnectAction->setStatusTip(tr("Login Xena"));
	connect(m_pConnectAction,SIGNAL(triggered()),this,SLOT(onLogin()));

	m_pImportAction = new QAction(tr("导入"),this);
	m_pImportAction->setIcon(QIcon(":/images/import.png"));
	m_pImportAction->setStatusTip(tr("Import an file"));
	connect(m_pImportAction,SIGNAL(triggered()),this,SLOT(onImport()));

	m_pExportAction = new QAction(tr("导出"),this);
	m_pExportAction->setIcon(QIcon(":/images/export.png"));
	m_pExportAction->setStatusTip(tr("Export the config to disk"));
	connect(m_pExportAction,SIGNAL(triggered()),this,SLOT(onExport()));

	m_pAboutAction = new QAction(tr("关于"),this);
	m_pAboutAction->setIcon(QIcon(":/images/about.png"));
	m_pAboutAction->setStatusTip(tr("About..."));
	connect(m_pAboutAction,SIGNAL(triggered()),this,SLOT(onAbout()));

	m_pQuitAction = new QAction(tr("退出"),this);
	m_pQuitAction->setIcon(QIcon(":/images/quit.png"));
	m_pQuitAction->setStatusTip(tr("Quit..."));
	connect(m_pQuitAction,SIGNAL(triggered()),this,SLOT(close()));
}

void XenaManager::createMenus()
{
	m_pFileMenu = menuBar()->addMenu(tr("文件"));
	m_pFileMenu->addAction(m_pImportAction);
}

void XenaManager::createToolBar()
{
	m_pToolBar = this->addToolBar("ToolBar");
	m_pToolBar->addAction(m_pConnectAction);
	m_pToolBar->addAction(m_pImportAction);
	m_pToolBar->addAction(m_pExportAction);
	m_pToolBar->addAction(m_pAboutAction);

	m_pQuitBar = this->addToolBar("QuitBar");
	m_pQuitBar->addAction(m_pQuitAction);
}

void XenaManager::createStatusBar()
{
	m_pStatusBarLabel = new QLabel;
	statusBar()->addWidget(m_pStatusBarLabel);
}

void XenaManager::initXenaManager()
{
	m_pRightWidget = new QWidget;
	m_pXenaTester = new CXenaTester;
	m_pLoginDialog = new LoginDialog(m_pXenaTester,this);
	m_pChassisViewWidget = new ChassisViewWidget(m_pXenaTester);
	m_pBaseConfigWidget[0] = new ChassisConfigWidget(m_pXenaTester,m_pRightWidget);
	m_pBaseConfigWidget[1] = new ModuleConfigWidget(m_pXenaTester,m_pRightWidget);
	m_pBaseConfigWidget[2] = new PortConfigWidget(m_pXenaTester,m_pChassisViewWidget,m_pRightWidget);
	m_pBaseConfigWidget[3] = new StreamConfigWidget(m_pXenaTester,m_pChassisViewWidget,m_pRightWidget);
	m_pCurrentBaseConfigWidget = m_pBaseConfigWidget[0];

	initUI();
}

void XenaManager::initUI()
{
	//this->setWindowTitle(tr("XenaManager--炫通电气测试版本"));
	createActions();
	//createMenus();
	createToolBar();
	createStatusBar();
	setMainLayout();

	m_pChassisViewWidget->setVisible(false);							// 隐藏左侧菜单treeview直到登陆成功
	for (int i = 0;i < SELECTIONTYPECOUNT;i++)
	{
		m_pBaseConfigWidget[i]->hide();
	}
}

void XenaManager::setMainCentralWidget(QLayout *layout)
{
	// 在QMainWindow已经有自己的布局了，不可以直接setLayout()
	QWidget *widget = new QWidget(this);
	widget->setLayout(layout);
	this->setCentralWidget(widget);
}

void XenaManager::setMainLayout()
{
	QVBoxLayout *layoutR = new QVBoxLayout;
	layoutR->addWidget(m_pBaseConfigWidget[0]);
	layoutR->addWidget(m_pBaseConfigWidget[1]);
	layoutR->addWidget(m_pBaseConfigWidget[2]);
	layoutR->addWidget(m_pBaseConfigWidget[3]);
	layoutR->setMargin(0);
	m_pRightWidget->setLayout(layoutR);

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(m_pChassisViewWidget);
	mainLayout->addWidget(m_pRightWidget);
	mainLayout->setMargin(0);

	setMainCentralWidget(mainLayout);
}

bool XenaManager::isNoStream()
{
    bool result = false;
    // 遍历板卡
    for (int moduleIndex = 0;moduleIndex < m_pXenaTester->chassis()->moduleCount();moduleIndex++)
    {
        // 遍历端口
        for (int portIndex = 0;portIndex < m_pXenaTester->chassis()->module(moduleIndex)->portCount();portIndex++)
        {
            if (m_pXenaTester->chassis()->module(moduleIndex)->port(portIndex)->streamCount())
            {
                return result;
            }
        }
    }

    return true;
}