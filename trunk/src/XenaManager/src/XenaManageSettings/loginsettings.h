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
 *      LoginSettings���ù��ߡ�
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
	
	// д������Ϣ�������ļ�
	static bool saveSettingsToFile();

	static bool setPort(const xuint32 & port);
    static bool setUserName(const std::string & userName);
	static bool setPassword(const std::string & password);
	static bool setIpAddress(const std::string & ipAddress);

private:
    LoginSettings();

private:
    void defaultSettings();
    //  �������ļ���������
    bool loadSettingsFromFile();
	
private:
    SLoginSettings m_settings;

	QDomDocument m_document; 
	QDomElement m_root;
	bool m_isModify;
};

#endif
