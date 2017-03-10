#pragma once

#include "softwareInfoSettings.h"

#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

#define XENAMANAGE_SETTINGS_FILE    ".\\version.xml"


CSoftwareInfoSettings* CSoftwareInfoSettings::getInstance()
{
    static CSoftwareInfoSettings settings;

    return & settings;
}

CSoftwareInfoSettings::CSoftwareInfoSettings()
{
	if(!loadSettingsFromFile())
	{
		defaultSettings();
	} 
}

void CSoftwareInfoSettings::defaultSettings()
{
    m_data.softwareName = "XenaManager";
    m_data.version = "V0.00.000";
}

//  从配置文件加载配置
bool CSoftwareInfoSettings::loadSettingsFromFile()
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

    m_data.softwareName = root.firstChildElement(QString("name")).attribute(QString("val"));
    QString mainVer = root.firstChildElement(QString("mainVer")).attribute(QString("val"));
    QString buildVer = root.firstChildElement(QString("buildVer")).attribute(QString("val"));
    if (buildVer.isEmpty()) {
        buildVer = "000";
    }
    m_data.version = mainVer + '.' + buildVer;
    
	return true;
}