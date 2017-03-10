#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class CXenaTester;
class QPushButton;
class QLabel;
class QLineEdit;
class QCheckBox;
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(CXenaTester * xenaTester,QWidget *parent = 0);
    
    bool loginResult() {return m_loginned;}
    
public slots:

private slots:
    void login();
    void onStateChanged(int state);

private:
	void initDialog();

private:
    QPushButton		*m_pLoginBtn;
    QPushButton		*m_pCancelBtn;

    QLabel			*m_pIpLabel;
    QLabel			*m_pPortLabel;
    QLabel			*m_pPwdLabel;
    QLabel          *m_pUserLabel;

    QLineEdit		*m_pIpLineEdit;
    QLineEdit		*m_pPortLineEdit;
    QLineEdit		*m_pPwdLineEdit;
    QLineEdit       *m_pUserLineEdit;

    QCheckBox       *m_pCustomCheckbox;

	CXenaTester		*m_pXenaTester;

    bool            m_loginned;
};

#endif // LOGINDIALOG_H
