#pragma once
#ifndef XENAMANAGESETTINGS_H
#define XENAMANAGESETTINGS_H
#include "XenaManager/src/inc/xtTypes.h"
#include <string>

/***********************************************************
 *
 *  @Description:
 *      XenaManager配置工具。
 *
************************************************************/

struct SXenaManageSettings {
    xuint32 keepAlive;
	xuint32 waitForRead;
	xuint32 waitConnected;
	xuint32 stateUpdaet;
	xuint32 streamRatePPS;
};

class XenaManageSettings {
public:
    static xuint32 breathTime() {return getInstance()->m_settings.keepAlive;}
	static xuint32 waitForReadReadyTime() {return getInstance()->m_settings.waitForRead;}
	static xuint32 waitConnectTime() {return getInstance()->m_settings.waitConnected;}
	static xuint32 stateUpdateTime() {return getInstance()->m_settings.stateUpdaet;}

private:
    XenaManageSettings();

    static XenaManageSettings* getInstance();

private:
    void defaultSettings();
    //  从配置文件加载配置
    bool loadSettingsFromFile();

private:
    SXenaManageSettings m_settings;
};

#endif
