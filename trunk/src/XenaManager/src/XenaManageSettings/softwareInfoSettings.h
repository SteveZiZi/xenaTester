#pragma once

#include <QString>

/***********************************************************
 *
 *  @Description:
 *      ����汾��Ϣ��OEM��Ϣ
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
    //  �������ļ���������
    bool loadSettingsFromFile();

private:
    Data m_data;
};
