#include "logindialog.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include "XenaManager/src/Communicate/xenaTester.h"
#include "XenaManager/src/licencesMgr/licencesMgr.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QBoxLayout>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QTime>

#define LICENCES_FILE_PATH  ".\\Licences\\"

#define MAXPORTCOUNT 65536
LoginDialog::LoginDialog(CXenaTester * xenaTester,QWidget *parent) :
    QDialog(parent),
    m_pXenaTester(xenaTester),
    m_loginned(false)
{
    initDialog();
    connect(m_pLoginBtn,SIGNAL(clicked()),this,SLOT(login()));
    connect(m_pCancelBtn,SIGNAL(clicked()),this,SLOT(reject()));
    connect(m_pCustomCheckbox,SIGNAL(stateChanged(int)),this,SLOT(onStateChanged(int)));
}

void LoginDialog::login()
{
    m_pLoginBtn->setEnabled(false);
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
        QString serialNo = m_pXenaTester->chassisSerialNo();
        serialNo.remove("\n");
        while (serialNo.length() < 8) {
            serialNo += 'x';                                            /*  不够长，用x填充             */
        }
        QString licencesFileName = LICENCES_FILE_PATH + serialNo + ".xml";
        if (!CLicencesMgr::isActive(licencesFileName, "XENA", serialNo)) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("No matched Licences"));
            qApp->exit(-1);
            return;
        }
        m_loginned = true;
        accept();
    }
    else
    {
        QMessageBox::warning(this,QObject::tr("Error"),QObject::tr("Login failed!"));
        m_pLoginBtn->setEnabled(true);
        m_loginned = false;
    }
}

void LoginDialog::onStateChanged(int state)
{
    if (Qt::Unchecked == state)
    {
        m_pPortLineEdit->setEnabled(false);
    }
    else if (Qt::Checked == state)
    {
        m_pPortLineEdit->setEnabled(true);
    }
}

void LoginDialog::initDialog()
{
    m_pLoginBtn = new QPushButton(tr("Login:"));
    m_pCancelBtn = new QPushButton(tr("Cancle:"));
    m_pIpLabel = new QLabel(tr("  IP:"));
    m_pUserLabel = new QLabel(tr("User:"));
    m_pPortLabel = new QLabel(tr("Port:"));
    m_pPwdLabel = new QLabel(tr("Secret:"));
    m_pCustomCheckbox = new QCheckBox(tr("Custom"));

    QRegExp regExpip("\\b(([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\b");  // ip正则表达式
    m_pIpLineEdit = new QLineEdit;
    m_pIpLineEdit->setValidator(new QRegExpValidator(regExpip,this));
    m_pIpLineEdit->setText(QString(LoginSettings::getIpAddress().data()));		//**********
    m_pUserLineEdit = new QLineEdit;
    m_pUserLineEdit->setMaxLength(8);
    m_pUserLineEdit->setText(QString(LoginSettings::getUserName().data()));
    m_pPortLineEdit = new QLineEdit;
    m_pPortLineEdit->setValidator(new QIntValidator(0,MAXPORTCOUNT,this));		// 端口号限制
    m_pPortLineEdit->setText(QString::number(LoginSettings::getPort()));		// ************
    m_pPortLineEdit->setFixedWidth(65);
    m_pPortLineEdit->setEnabled(false);
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
    portLayout->addWidget(m_pCustomCheckbox);
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


