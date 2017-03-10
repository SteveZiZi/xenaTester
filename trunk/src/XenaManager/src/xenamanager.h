#ifndef XENAMANAGER_H
#define XENAMANAGER_H

#include <QtGui/QMainWindow>
#include "ui_xenamanager.h"
#include "XenaManager/src/treeView/ChassisViewWidget.h"

#define SELECTIONTYPECOUNT 4

class QWidget;
class QLabel;
class QAction;
class QToolBar;
class LoginDialog;
class ChassisViewWidget;
class BaseConfigWidget;
class ConfigWidget;
class CXenaTester;
class QSplitter;
class XenaManager : public QMainWindow
{
    Q_OBJECT

public:
    XenaManager(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~XenaManager();

protected:
	void closeEvent(QCloseEvent *event);								// 关闭之前保存窗口大小信息

private slots:
	void onLogin();
	void onAbout();														// 关于
	void onUpdateConfigWidget(S_CURRENT_INDEX currenIndex);				// 更新配置界面信息

private:
	void readSettings();												// 通过QSetting读取窗口大小的配置信息
	void writeSettings();												// 通过QSetting保存窗口大小的配置信息
	void createActions();												// 创建所有的Action
	void createMenus();													// 创建所有的Menu
	void createToolBar();												// 创建工具栏
	void createStatusBar();
	void initXenaManager(); 
	void initUI();
	void setMainCentralWidget(QLayout *layout);

    bool isNoStream();

private:
    Ui::XenaManagerClass ui;

    QAction             *m_pConnectAction;								// 连接
    QAction             *m_pAboutAction;								// 关于
    QAction             *m_pQuitAction;									// 退出

    QMenu               *m_pFileMenu;
    QToolBar            *m_pToolBar;
    QToolBar            *m_pQuitBar;
    QLabel              *m_pStatusBarLabel;

    QWidget             *m_pRightWidget;
    ChassisViewWidget   *m_pChassisViewWidget;
    BaseConfigWidget    *m_pBaseConfigWidget[SELECTIONTYPECOUNT];
    BaseConfigWidget    *m_pCurrentBaseConfigWidget;
    QSplitter           * m_pSplitter;

    CXenaTester         *m_pXenaTester;
};

#endif // XENAMANAGER_H
