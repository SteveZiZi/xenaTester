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
	void closeEvent(QCloseEvent *event);								// �ر�֮ǰ���洰�ڴ�С��Ϣ

private slots:
	void onLogin();
	void onAbout();														// ����
	void onUpdateConfigWidget(S_CURRENT_INDEX currenIndex);				// �������ý�����Ϣ

private:
	void readSettings();												// ͨ��QSetting��ȡ���ڴ�С��������Ϣ
	void writeSettings();												// ͨ��QSetting���洰�ڴ�С��������Ϣ
	void createActions();												// �������е�Action
	void createMenus();													// �������е�Menu
	void createToolBar();												// ����������
	void createStatusBar();
	void initXenaManager(); 
	void initUI();
	void setMainCentralWidget(QLayout *layout);

    bool isNoStream();

private:
    Ui::XenaManagerClass ui;

    QAction             *m_pConnectAction;								// ����
    QAction             *m_pAboutAction;								// ����
    QAction             *m_pQuitAction;									// �˳�

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
