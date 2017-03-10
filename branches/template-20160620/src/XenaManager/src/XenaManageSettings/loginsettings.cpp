#pragma once

#include "loginsettings.h"

#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

#define READWRITE_SETTINGS_FILE    "ReadWriteSettings.xml"
#define PORT "port"
#define USERNAME "userName"
#define IPADDRESS "ipAddress"
#define PASSWORD "password"


LoginSettings* LoginSettings::getLoginSettings()
{
    static LoginSettings settings;

    return & settings;
}

LoginSettings::LoginSettings()
{
	m_isModify = false;
	if(!loadSettingsFromFile())
	{
		defaultSettings();
	} 
}

void LoginSettings::defaultSettings()
{
    m_settings.port = 22166;
    m_settings.userName = "XT";
    m_settings.ipAddress = "192.168.0.202";
	m_settings.password = "xena";
}

//  从配置文件加载配置
bool LoginSettings::loadSettingsFromFile()
{
    QFile file(READWRITE_SETTINGS_FILE);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QString string(QString("LoginSettings::load open %1 fail.").arg(READWRITE_SETTINGS_FILE));
        qDebug() << string;
        return false;  
    }  

    QString error;  
    int row = 0, column = 0;  
    if(!m_document.setContent(&file, false, &error, &row, &column)) {  
        qDebug() << QString("load parse file failed at line row and column %1,%2").arg(row).arg(column);
        return false;  
    }  
    file.close();

    if(m_document.isNull()) {  
        qDebug() << "LoginSettings::load document is null!";
        return false;  
    }  

    m_root = m_document.documentElement();  
    if (m_root.isNull()) {
        return false;
    }

    m_settings.port = m_root.firstChildElement(QString(PORT)).attribute(QString("val")).toUInt();
    m_settings.userName = m_root.firstChildElement(QString(USERNAME)).attribute(QString("val")).toLocal8Bit().data();
    m_settings.ipAddress = m_root.firstChildElement(QString(IPADDRESS)).attribute(QString("val")).toLocal8Bit().data();
	m_settings.password = m_root.firstChildElement(QString(PASSWORD)).attribute(QString("val")).toLocal8Bit().data();
    
	return true;
}

bool LoginSettings::saveSettingsToFile()
{
	LoginSettings *settings = getLoginSettings();

	if (!settings->m_isModify) {
		return true;
	}

	QFile file(READWRITE_SETTINGS_FILE);  
	if(!file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text)) {
		QString string(QString("ReadWriteSettings::save open %1 fail.").arg(READWRITE_SETTINGS_FILE));
		qDebug() << string;
		return false;  
	}  

	QByteArray text(settings->m_document.toByteArray());
	file.write(text.data(), text.size());
	file.flush();
	file.close();

	return true;
}


bool LoginSettings::setPort(const xuint32 & port)
{
	LoginSettings *settings = getLoginSettings();

	if (settings->m_root.isNull()) {
		return false;
	}
	QDomElement ele = settings->m_root.firstChildElement(QString(PORT));
	if (ele.isNull()) {
		ele = settings->m_document.createElement(QString(PORT));
		settings->m_root.appendChild(ele);
	}
	ele.setAttribute(QString("val"), QString::number(port));

	settings->m_isModify = true;

	settings->m_settings.port = port;

	return true;
}

bool LoginSettings::setUserName(const std::string & userName)
{
    LoginSettings *settings = getLoginSettings();

    if (settings->m_root.isNull()) {
        return false;
    }
    QDomElement ele = settings->m_root.firstChildElement(QString(USERNAME));
    if (ele.isNull()) {
        ele = settings->m_document.createElement(QString(USERNAME));
        settings->m_root.appendChild(ele);
    }
    ele.setAttribute(QString("val"), QString(userName.data()));

    settings->m_isModify = true;

    settings->m_settings.userName = userName;

    return true;
}

bool LoginSettings::setPassword(const std::string & password)
{
	LoginSettings *settings = getLoginSettings();

	if (settings->m_root.isNull()) {
		return false;
	}
	QDomElement ele = settings->m_root.firstChildElement(QString(PASSWORD));
	if (ele.isNull()) {
		ele = settings->m_document.createElement(QString(PASSWORD));
		settings->m_root.appendChild(ele);
	}
	ele.setAttribute(QString("val"), QString(password.data()));

	settings->m_isModify = true;

	settings->m_settings.password = password;

	return true;
}

bool LoginSettings::setIpAddress(const std::string & ipAddress)
{
	LoginSettings *settings = getLoginSettings();

	if (settings->m_root.isNull()) {
		return false;
	}
	QDomElement ele = settings->m_root.firstChildElement(QString(IPADDRESS));
	if (ele.isNull()) {
		ele = settings->m_document.createElement(QString(IPADDRESS));
		settings->m_root.appendChild(ele);
	}
	ele.setAttribute(QString("val"), QString(ipAddress.data()));

	settings->m_isModify = true;

	settings->m_settings.ipAddress = ipAddress;

	return true;
}