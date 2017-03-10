#pragma once

#include <QString>

/***********************************************************
 *
 *  @Description:
 *      软件版本信息、OEM信息
 *
************************************************************/

class CSoftwareInfoSettings {
public:

    struct Data {
        QString softwareName;
        QString version;
    };

    static CSoftwareInfoSettings* getInstance();

    QString softwareName() {return m_data.softwareName;}
    QString version() {return m_data.version;}

private:
    CSoftwareInfoSettings();


private:
    void defaultSettings();
    //  从配置文件加载配置
    bool loadSettingsFromFile();

private:
    Data m_data;
};
