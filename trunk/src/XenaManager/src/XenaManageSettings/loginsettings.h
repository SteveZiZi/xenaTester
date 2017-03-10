#pragma once
#ifndef READWRITESETTINGS_H
#define READWRITESETTINGS_H
#include "XenaManager/src/inc/xtTypes.h"
#include <string>

#include <QDomDocument>
#include <QDomElement>

/***********************************************************
 *
 *  @Description:
 *      LoginSettings配置工具。
 *
************************************************************/

struct SLoginSettings {
    xuint32 port;
    std::string userName;
    std::string ipAddress;
	std::string password;
};

class LoginSettings {
public:
	static LoginSettings* getLoginSettings();

    static const std::string getUserName() {return getLoginSettings()->m_settings.userName;}
    static const xuint32& getPort() {return getLoginSettings()->m_settings.port;}
    static const std::string& getIpAddress() {return getLoginSettings()->m_settings.ipAddress;}
	static const std::string& getPassword() {return getLoginSettings()->m_settings.password;}
	
	// 写配置信息到配置文件
	static bool saveSettingsToFile();

	static bool setPort(const xuint32 & port);
    static bool setUserName(const std::string & userName);
	static bool setPassword(const std::string & password);
	static bool setIpAddress(const std::string & ipAddress);

private:
    LoginSettings();

private:
    void defaultSettings();
    //  从配置文件加载配置
    bool loadSettingsFromFile();
	
private:
    SLoginSettings m_settings;

	QDomDocument m_document; 
	QDomElement m_root;
	bool m_isModify;
};

#endif
