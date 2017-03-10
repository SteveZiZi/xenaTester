#include "stdafx.h"
#include "utility.h"



/**
 *      获取当前UTC时间的秒数
 *
 *  \return
 *      UTC_TIME_t          返回UTC时间的秒数信息
 */
extern UTC_TIME_t DM_GetUTC()
{
    __time64_t tt64;
    _time64(&tt64);

    UTC_TIME_t utt = {uint32_t(tt64), 0};

    return utt;
}

/**
 *      从UTC时间获取本地时间
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      SYSTEMTIME          返回本地时间
 */
extern SYSTEMTIME DM_GetLocalTimeFromUTC( UTC_TIME_t t )
{
    SYSTEMTIME  sysTime;
    CTime       stTm1((__time64_t)t.uSecondSinceEpoch);
    tm          stTm2;

    stTm1.GetLocalTm(&stTm2);
    sysTime.wYear           = (WORD)stTm2.tm_year + 1900;
    sysTime.wMonth          = (WORD)stTm2.tm_mon + 1;
    sysTime.wDay            = (WORD)stTm2.tm_mday;
    sysTime.wHour           = (WORD)stTm2.tm_hour;
    sysTime.wMinute         = (WORD)stTm2.tm_min;
    sysTime.wSecond         = (WORD)stTm2.tm_sec;
    sysTime.wDayOfWeek      = (WORD)stTm2.tm_wday;
    
    sysTime.wMilliseconds   = (WORD)(t.uFractionOfSecond/1000);

    return sysTime;
}

/**
 *      从UTC获取本地时间的字符串描述
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      CString             返回本地时间的字符串描述
 */
extern CString DM_GetLocalTimeStrFromUTC( UTC_TIME_t t )
{
    SYSTEMTIME st = DM_GetLocalTimeFromUTC(t);

    CString csTime;
    csTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    return csTime;
}

extern SYSTEMTIME DM_GetSystemTimeFromUTC( int64_t seconds, int32_t fraction )
{
    SYSTEMTIME  sysTime;
    CTime       stTm1(seconds);
    tm          stTm2;

    stTm1.GetGmtTm(&stTm2);
    sysTime.wYear           = (WORD)stTm2.tm_year + 1900;
    sysTime.wMonth          = (WORD)stTm2.tm_mon + 1;
    sysTime.wDay            = (WORD)stTm2.tm_mday;
    sysTime.wHour           = (WORD)stTm2.tm_hour;
    sysTime.wMinute         = (WORD)stTm2.tm_min;
    sysTime.wSecond         = (WORD)stTm2.tm_sec;
    sysTime.wDayOfWeek      = (WORD)stTm2.tm_wday;

    sysTime.wMilliseconds   = (WORD)(fraction/1000);

    return sysTime;
}


extern CString DM_GetSystemTimeStrFromUTC( UTC_TIME_t t)
{
    SYSTEMTIME st = DM_GetSystemTimeFromUTC(t.uSecondSinceEpoch, t.uFractionOfSecond);

    CString csTime;
    csTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    return csTime;
}

extern UTC_TIME_t DM_GetUTCFromSystemTime( LPSYSTEMTIME lpSystemTime)
{
    FILETIME ftTime;
    SystemTimeToFileTime(lpSystemTime, &ftTime);

    UTC_TIME_t utt;
    utt.uSecondSinceEpoch = ftTime.dwHighDateTime;
    utt.uFractionOfSecond = ftTime.dwLowDateTime;

    return utt;
}

extern BOOL DM_SystemTimeToTzSpecificLocalTime(LPSYSTEMTIME lpUniversalTime,LPSYSTEMTIME lpLocalTime)
{
    if(lpUniversalTime == NULL || lpLocalTime == NULL)
    {
        //如果指针为空，则没有必要进行任何计算
        return FALSE;
    }
    
    TIME_ZONE_INFORMATION tzInfo;
    ::ZeroMemory(&tzInfo, sizeof(tzInfo));
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    (void)dstFlag;

    //将UTC时间由SYSTEMTIME转换为FILETIME格式
    FILETIME ftUniversalTime = {0};
    SystemTimeToFileTime(lpUniversalTime,&ftUniversalTime);

    //将FILETIME格式时间的数值存储到一个DWORD64变量中
    DWORD64 ddwUniversalTime = ftUniversalTime.dwHighDateTime;
    ddwUniversalTime = ddwUniversalTime << 32;
    ddwUniversalTime += ftUniversalTime.dwLowDateTime;


    //因为FILETIME的时间单位是100个亿分之一秒，然后TIME_ZONE_INFORMATION的时间单位是分，所以这里需要乘以600000000 
    DWORD64 ddwBias = ((DWORD64)-tzInfo.Bias) * 600000000; 

    //转换公式为：LOCAL_TIME = UTC - BIAS
    DWORD64 ddwLocalTime = 0;
    ddwLocalTime = ddwUniversalTime + ddwBias;

    //将DWORD64数值转换为FILETIME格式
    FILETIME ftLocalTime = {0};
    ftLocalTime.dwLowDateTime = static_cast<DWORD>(ddwLocalTime);
    ftLocalTime.dwHighDateTime = static_cast<DWORD>(ddwLocalTime >> 32);

    //将FILETIME数值转换为SYSTEMTIME格式并返回
    return FileTimeToSystemTime(&ftLocalTime,lpLocalTime);
}

extern BOOL DM_TzSpecificLocalTimeToSystemTime(LPSYSTEMTIME lpLocalTime, LPSYSTEMTIME lpUniversalTime)
{
    if(lpLocalTime == NULL || lpUniversalTime == NULL)
    {
        //如果指针为空，则没有必要进行任何计算
        return FALSE;
    }

    TIME_ZONE_INFORMATION tzInfo;
    ::ZeroMemory(&tzInfo, sizeof(tzInfo));
    GetTimeZoneInformation(&tzInfo);

    //将UTC时间由SYSTEMTIME转换为FILETIME格式
    FILETIME ftLocalTime = {0};
    SystemTimeToFileTime(lpLocalTime,&ftLocalTime);

    //将FILETIME格式时间的数值存储到一个DWORD64变量中
    DWORD64 ddwLocalTime = ftLocalTime.dwHighDateTime;
    ddwLocalTime = ddwLocalTime << 32;
    ddwLocalTime += ftLocalTime.dwLowDateTime;


    //因为FILETIME的时间单位是100个亿分之一秒，然后TIME_ZONE_INFORMATION的时间单位是分，所以这里需要乘以600000000 
    DWORD64 ddwBias = ((DWORD64)-tzInfo.Bias) * 600000000; 

    //转换公式为：LOCAL_TIME = UTC - BIAS
    DWORD64 ddwUniversalTime = 0;
    ddwUniversalTime = ddwLocalTime - ddwBias;

    //将DWORD64数值转换为FILETIME格式
    FILETIME ftUniversalTime = {0};
    ftUniversalTime.dwLowDateTime = static_cast<DWORD>(ddwUniversalTime);
    ftUniversalTime.dwHighDateTime = static_cast<DWORD>(ddwUniversalTime >> 32);

    //将FILETIME数值转换为SYSTEMTIME格式并返回
    return FileTimeToSystemTime(&ftUniversalTime,lpUniversalTime);
}