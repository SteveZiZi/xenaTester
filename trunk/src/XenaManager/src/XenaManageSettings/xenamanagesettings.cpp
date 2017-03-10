#pragma once

#include "xenamanagesettings.h"

#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

#define XENAMANAGE_SETTINGS_FILE    ".\\config\\XenaManageSettings.xml"


XenaManageSettings* XenaManageSettings::getInstance()
{
    static XenaManageSettings settings;

    return & settings;
}

XenaManageSettings::XenaManageSettings()
{
	if(!loadSettingsFromFile())
	{
		defaultSettings();
	} 
}

void XenaManageSettings::defaultSettings()
{
    m_settings.keepAlive = 30000;
	m_settings.waitForRead = 3000;
    m_settings.waitForContinueRead = 1000;
    m_settings.waitConnected = 3000;
	m_settings.stateUpdaet = 1000;
}

//  从配置文件加载配置
bool XenaManageSettings::loadSettingsFromFile()
{
    QFile file(XENAMANAGE_SETTINGS_FILE);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QString string(QString("XenaManageconfig::load open %1 fail.").arg(XENAMANAGE_SETTINGS_FILE));
        qDebug() << string;
        return false;  
    }  

    QDomDocument document;  
    QString error;  
    int row = 0, column = 0;  
    if(!document.setContent(&file, false, &error, &row, &column)) {  
        qDebug() << QString("load parse file failed at line row and column %1,%2").arg(row).arg(column);
        return false;  
    }  
    file.close();

    if(document.isNull()) {  
        qDebug() << "XenaManageconfig::load document is null!";
        return false;  
    }  

    QDomElement root = document.documentElement();  
    if (root.isNull()) {
        return false;
    }

    m_settings.keepAlive = root.firstChildElement(QString("keepAlive")).attribute(QString("val")).toUInt();
    m_settings.waitForRead = root.firstChildElement(QString("waitForRead")).attribute(QString("val")).toUInt();
    m_settings.waitForContinueRead = root.firstChildElement(QString("waitForContinueRead")).attribute(QString("val")).toUInt();
	m_settings.waitConnected = root.firstChildElement(QString("waitConnected")).attribute(QString("val")).toUInt();
	m_settings.stateUpdaet = root.firstChildElement(QString("stateUpdate")).attribute(QString("val")).toUInt();
    
	return true;
}