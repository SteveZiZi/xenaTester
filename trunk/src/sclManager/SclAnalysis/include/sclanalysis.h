#ifndef _SCL_ANALYSIS_H_
#define _SCL_ANALYSIS_H_

// 在当前不使用SCL文件解析库的动态链接版本时，需在该头文件之前添加_SCLANALYSIS_STATICLINK宏,即
// #define _SCLANALYSIS_STATICLINK
// #include "sclanalysis.h"
// 或者在预处理器中添加

#if defined(_LIB) || defined(_SCLANALYSIS_STATICLINK)
#define SCLMANAGER_API

#else
#ifdef SCLMANAGER_EXPORTS
#define SCLMANAGER_API __declspec(dllexport)
#else
#define SCLMANAGER_API __declspec(dllimport)
#endif // SCLMANAGER_EXPORTS

#endif // defined(_Lib) || defined(_SCLANALYSIS_STATICLINK)


#endif // _SCL_ANALYSIS_H_
