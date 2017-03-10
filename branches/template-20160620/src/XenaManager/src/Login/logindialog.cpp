#include "logindialog.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/licencesMgr/licencesMgr.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QBoxLayout>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>

#define LICENCES_FILE_PATH  ".\\Licences\\"

#define MAXPORTCOUNT 65536
LoginDialog::LoginDialog(CXenaTester * xenaTester,QWidget *parent) :
    QDialog(parent),
	m_pXenaTester(xenaTester)
{
    initDialog();
    connect(m_pLoginBtn,SIGNAL(clicked()),this,SLOT(login()));
    connect(m_pCancelBtn,SIGNAL(clicked()),this,SLOT(close()));
}

void LoginDialog::login()
{
	LoginSettings::setPort(m_pPortLineEdit->text().toUInt());
    LoginSettings::setUserName(m_pUserLineEdit->text().toStdString());
	LoginSettings::setIpAddress(m_pIpLineEdit->text().toStdString());
	LoginSettings::setPassword(m_pPwdLineEdit->text().toStdString());
	LoginSettings::saveSettingsToFile();

	m_pXenaTester->login(m_pIpLineEdit->text(),m_pPortLineEdit->text().toInt(),
        m_pUserLineEdit->text(),m_pPwdLineEdit->text());

	if (m_pXenaTester->isLogin())
	{
        m_pXenaTester->queryChassisInfo();
#ifndef _DEBUG
        QString serialNo = m_pXenaTester->chassisSerialNo();
        serialNo.remove("\n");
        QString licencesFileName = LICENCES_FILE_PATH + serialNo + ".xml";
        if (!CLicencesMgr::isActive(licencesFileName)) {
            QMessageBox::warning(0, QObject::tr("����"), QObject::tr("δ�ҵ���Ӧ�豸��licences"));
            qApp->exit(-1);
            return;
        }
#endif
		emit loginResult(true);
	}
	else
	{
		emit loginResult(false);
	}
}

void LoginDialog::initDialog()
{
    m_pLoginBtn = new QPushButton(tr("��½"));
    m_pCancelBtn = new QPushButton(tr("ȡ��"));
    m_pIpLabel = new QLabel(tr("I  P:"));
    m_pUserLabel = new QLabel(tr("�û�:"));
    m_pPortLabel = new QLabel(tr("�˿�:"));
    m_pPwdLabel = new QLabel(tr("����:"));


    QRegExp regExpip("\\b(([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\b");  // ip������ʽ
    m_pIpLineEdit = new QLineEdit;
    m_pIpLineEdit->setValidator(new QRegExpValidator(regExpip,this));
    m_pIpLineEdit->setText(QString(LoginSettings::getIpAddress().data()));		//**********
    m_pUserLineEdit = new QLineEdit;
    m_pUserLineEdit->setMaxLength(8);
    m_pUserLineEdit->setText(QString(LoginSettings::getUserName().data()));
    m_pPortLineEdit = new QLineEdit;
    m_pPortLineEdit->setValidator(new QIntValidator(0,MAXPORTCOUNT,this));		// �˿ں�����
    m_pPortLineEdit->setText(QString::number(LoginSettings::getPort()));		// ************
    m_pPwdLineEdit = new QLineEdit;
    m_pPwdLineEdit->setEchoMode(QLineEdit::Password);
    m_pPwdLineEdit->setText(QString(LoginSettings::getPassword().data()));

    QHBoxLayout *ipLayout = new QHBoxLayout;
    ipLayout->addWidget(m_pIpLabel);
    ipLayout->addStretch();
    ipLayout->addWidget(m_pIpLineEdit);
    ipLayout->addStretch();

    QHBoxLayout *userLayout = new QHBoxLayout;
    userLayout->addWidget(m_pUserLabel);
    userLayout->addStretch();
    userLayout->addWidget(m_pUserLineEdit);
    userLayout->addStretch();

    QHBoxLayout *portLayout = new QHBoxLayout;
    portLayout->addWidget(m_pPortLabel);
    portLayout->addStretch();
    portLayout->addWidget(m_pPortLineEdit);
    portLayout->addStretch();

    QHBoxLayout *pwdLayout = new QHBoxLayout;
    pwdLayout->addWidget(m_pPwdLabel);
    pwdLayout->addStretch();
    pwdLayout->addWidget(m_pPwdLineEdit);
    pwdLayout->addStretch();

    QHBoxLayout *buttomLayout = new QHBoxLayout;
    buttomLayout->addStretch();
    buttomLayout->addWidget(m_pLoginBtn);
    buttomLayout->addWidget(m_pCancelBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(ipLayout);
    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(portLayout);
    mainLayout->addLayout(pwdLayout);
    mainLayout->addLayout(buttomLayout);

    this->setLayout(mainLayout);

    this->setFixedWidth(sizeHint().width());
    this->setFixedHeight(sizeHint().height());
}


