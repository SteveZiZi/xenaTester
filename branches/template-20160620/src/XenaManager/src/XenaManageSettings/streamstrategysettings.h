#pragma once
#ifndef STREAMSTRATEGYSETTINGS_H
#define STREAMSTRATEGYSETTINGS_H
#include "XenaManager/src/inc/xtTypes.h"
#include <string>


/***********************************************************
 *
 *  @Description:
 *      StreamStrategySettings配置工具。
 *
************************************************************/

struct SStreamStrategySettings {
    bool streamEnable;
	bool packetStrategyEnable;
	xuint32 packets;
	bool stepStrategyEnable;
	double beginRate;
	double endRate;
	double stepRate;
	xuint32 loopTimes;
};

class StreamStrategySettings {
public:
    static bool getStreamEnable() {return getInstance()->m_settings.streamEnable;}
    static bool getPacketStrategyEnable() {return getInstance()->m_settings.packetStrategyEnable;}
	static xuint32 getPackets() {return getInstance()->m_settings.packets;}
	static bool getStepStrategyEnabel() {return getInstance()->m_settings.stepStrategyEnable;}
	static double getBeginRate() {return getInstance()->m_settings.beginRate;}
	static double getEndRate() {return getInstance()->m_settings.endRate;}
	static double getStepRate() {return getInstance()->m_settings.stepRate;}
	static xuint32 getLoopTimes() {return getInstance()->m_settings.loopTimes;}

private:
    StreamStrategySettings();

    static StreamStrategySettings* getInstance();

private:
    void defaultSettings();
    //  从配置文件加载配置
    bool loadSettingsFromFile();

private:
    SStreamStrategySettings m_settings;
};

#endif // STREAMSTRATEGYSETTINGS_H
