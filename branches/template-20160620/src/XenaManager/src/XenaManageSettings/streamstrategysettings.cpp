#pragma once
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

#include "streamstrategysettings.h"

#define STREAMSTRATEGYSETTINGS    "StreamStrategySettings.xml"

StreamStrategySettings::StreamStrategySettings()
{
	if(!loadSettingsFromFile())
	{
		defaultSettings();
	} 
}

StreamStrategySettings* StreamStrategySettings::getInstance()
{
	static StreamStrategySettings settings;

	return &settings;
}

void StreamStrategySettings::defaultSettings()
{
	m_settings.streamEnable = true;
	m_settings.packetStrategyEnable = false;
	m_settings.packets = 0;
	m_settings.stepStrategyEnable = false;
	m_settings.beginRate = 1.0;
	m_settings.endRate = 100.0;
	m_settings.stepRate = 1.0;
	m_settings.loopTimes = 0;
}

//  从配置文件加载配置
bool StreamStrategySettings::loadSettingsFromFile()
{
	QFile file(STREAMSTRATEGYSETTINGS);
	if(!file.open(QFile::ReadOnly | QFile::Text)) {
		QString string(QString("XenaManageconfig::load open %1 fail.").arg(STREAMSTRATEGYSETTINGS));
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

	
	m_settings.streamEnable = root.firstChildElement(QString("streamEnable")).attribute(QString("val")).toUInt();
	m_settings.packetStrategyEnable = root.firstChildElement(QString("packetStrategyEnable")).attribute(QString("val")).toUInt();
	m_settings.packets = root.firstChildElement(QString("packets")).attribute(QString("val")).toUInt();
	m_settings.stepStrategyEnable = root.firstChildElement(QString("stepStrategyEnable")).attribute(QString("val")).toUInt();
	m_settings.beginRate = root.firstChildElement(QString("beginRate")).attribute(QString("val")).toDouble();
	m_settings.endRate = root.firstChildElement(QString("endRate")).attribute(QString("val")).toDouble();
	m_settings.stepRate = root.firstChildElement(QString("stepRate")).attribute(QString("val")).toDouble();
	m_settings.loopTimes = root.firstChildElement(QString("loopTimes")).attribute(QString("val")).toUInt();
	return true;
}

