#include "xenamanager.h"
#include "XenaManager/src/Login/logindialog.h"
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/chassisconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/moduleconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/portconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/streamconfigwidget.h"
#include "XenaManager/src/dialog/configDialog/baseconfigwidget.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/XenaManageSettings/softwareInfoSettings.h"
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

	connect(m_pChassisViewWidget,SIGNAL(updateConfigWidget(S_CURRENT_INDEX)),this,SLOT(onUpdateConfigWidget(S_CURRENT_INDEX)));    // 点击左侧treeView更新右侧配置界面
}

XenaManager::~XenaManager()
{
    delete m_pXenaTester;
}

void XenaManager::onLogin()
{
    LoginDialog loginDialog(m_pXenaTester,this);
    m_pStatusBarLabel->setText(tr("Login..."));
    int result = loginDialog.exec();
    m_pStatusBarLabel->setText("");

    if (QDialog::Accepted == result)
    {
        if (loginDialog.loginResult())
        {
            m_pConnectAction->setEnabled(false);
            m_pChassisViewWidget->initTreeViewWidgetData();					// 将机箱信息显示出来
            m_pChassisViewWidget->setVisible(true);
            m_pCurrentBaseConfigWidget->setVisible(true);
        }
        else
        {
            m_pConnectAction->setEnabled(true);
        }
    }
}

void XenaManager::onAbout()
{
    QString name = tr("Software Name:") + CSoftwareInfoSettings::getInstance()->softwareName();
    QString version = tr("Software Version:") + CSoftwareInfoSettings::getInstance()->version();
    QString info = name + "\r\n" + version;
	QMessageBox aboutMessageBox(QMessageBox::Information,tr("About"),info);
	aboutMessageBox.setStandardButtons(QMessageBox::Ok);
	aboutMessageBox.setButtonText(QMessageBox::Ok,tr("OK"));
	aboutMessageBox.exec();
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
#if 0
    if (!isNoStream())
    {
        QMessageBox::warning(0,tr("Warnning"),tr("Please Clean All Stream"),QString(tr("OK")));
        event->ignore();
    }
    else
    {
        writeSettings();
        event->accept();
    }
#else 
    writeSettings();
    event->accept();
#endif
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
	m_pConnectAction  = new QAction(tr("Login"),this);
	m_pConnectAction->setIcon(QIcon(":/images/connect.png"));
	m_pConnectAction->setStatusTip(tr("Login Xena"));
	connect(m_pConnectAction,SIGNAL(triggered()),this,SLOT(onLogin()));

	m_pAboutAction = new QAction(tr("About"),this);
	m_pAboutAction->setIcon(QIcon(":/images/about.png"));
	m_pAboutAction->setStatusTip(tr("About..."));
	connect(m_pAboutAction,SIGNAL(triggered()),this,SLOT(onAbout()));

	m_pQuitAction = new QAction(tr("Quit"),this);
	m_pQuitAction->setIcon(QIcon(":/images/quit.png"));
	m_pQuitAction->setStatusTip(tr("Quit..."));
	connect(m_pQuitAction,SIGNAL(triggered()),this,SLOT(close()));
}

void XenaManager::createMenus()
{
	m_pFileMenu = menuBar()->addMenu(tr("File"));
}

void XenaManager::createToolBar()
{
	m_pToolBar = this->addToolBar("ToolBar");
	m_pToolBar->addAction(m_pConnectAction);
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
    m_pSplitter = new QSplitter(Qt::Horizontal);

    m_pXenaTester = new CXenaTester();
    m_pChassisViewWidget = new ChassisViewWidget(m_pXenaTester, m_pSplitter);

	m_pRightWidget = new QWidget(m_pSplitter);
	m_pBaseConfigWidget[0] = new ChassisConfigWidget(m_pXenaTester,m_pRightWidget);
	m_pBaseConfigWidget[1] = new ModuleConfigWidget(m_pXenaTester,m_pRightWidget);
	m_pBaseConfigWidget[2] = new PortConfigWidget(m_pXenaTester,m_pChassisViewWidget,m_pRightWidget);
	m_pBaseConfigWidget[3] = new StreamConfigWidget(m_pXenaTester,m_pChassisViewWidget,m_pRightWidget);
	m_pCurrentBaseConfigWidget = m_pBaseConfigWidget[0];

	initUI();


    QVBoxLayout *layoutR = new QVBoxLayout;
    layoutR->addWidget(m_pBaseConfigWidget[0]);
    layoutR->addWidget(m_pBaseConfigWidget[1]);
    layoutR->addWidget(m_pBaseConfigWidget[2]);
    layoutR->addWidget(m_pBaseConfigWidget[3]);
    layoutR->setMargin(0);
    m_pRightWidget->setLayout(layoutR);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pSplitter);
    mainLayout->setMargin(0);

    setMainCentralWidget(mainLayout);
}

void XenaManager::initUI()
{
	this->setWindowTitle(tr("XenaSCD"));
	createActions();
	//createMenus();
	createToolBar();
	createStatusBar();

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