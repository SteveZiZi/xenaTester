#ifndef _SCL_ANALYSIS_H_
#define _SCL_ANALYSIS_H_

// �ڵ�ǰ��ʹ��SCL�ļ�������Ķ�̬���Ӱ汾ʱ�����ڸ�ͷ�ļ�֮ǰ���_SCLANALYSIS_STATICLINK��,��
// #define _SCLANALYSIS_STATICLINK
// #include "sclanalysis.h"
// ������Ԥ�����������

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
