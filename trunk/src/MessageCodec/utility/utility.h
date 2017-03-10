#pragma once

#include "inc\utcTime.h"

/**
 *      获取当前UTC时间的秒数
 *
 *  \return
 *      UTC_TIME_t          返回UTC时间的秒数信息
 */
extern UTC_TIME_t DM_GetUTC();

/**
 *      从UTC时间获取本地时间
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      SYSTEMTIME          返回本地时间
 */
extern SYSTEMTIME DM_GetLocalTimeFromUTC(UTC_TIME_t t);

/**
 *      从UTC获取本地时间的字符串描述
 *
 *  \param[in]
 *      t                   UTC时间秒数，从1970年午夜开始
 *  return
 *      CString             返回本地时间的字符串描述
 */
extern CString DM_GetLocalTimeStrFromUTC(UTC_TIME_t t);


/**
 *      获取UTC时间
 *
 *  \param[in]
 *      seconds         秒
 *  \param[in]
 *      fraction        微秒
 */
extern SYSTEMTIME DM_GetSystemTimeFromUTC(int64_t seconds, int32_t fraction);

/**
 *      获取UTC时间的字符串描述
 */
extern CString DM_GetSystemTimeStrFromUTC(UTC_TIME_t t);


/**
 *      系统时间转化为UTC时间
 */
extern UTC_TIME_t DM_GetUTCFromSystemTime( LPSYSTEMTIME lpSystemTime);

/**
 *      UTC时间转化为本地时间
 */
extern BOOL DM_SystemTimeToTzSpecificLocalTime(LPSYSTEMTIME lpUniversalTime, LPSYSTEMTIME lpLocalTime);

/**
 *      本地时间转化为UTC时间
 */
extern BOOL DM_TzSpecificLocalTimeToSystemTime(LPSYSTEMTIME lpLocalTime, LPSYSTEMTIME lpUniversalTime);