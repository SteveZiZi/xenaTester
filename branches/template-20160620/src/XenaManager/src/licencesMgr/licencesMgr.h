#pragma once

#include <QString>

class CLicencesMgr
{
public:
    static bool isActive(const QString &licencesFile);
};