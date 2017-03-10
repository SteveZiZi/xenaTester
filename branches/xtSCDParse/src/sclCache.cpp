/// @file
///     sclcache.cpp
///
/// @brief
///     SCL解析数据Cache的实例接口
///
/// @note
///     目前仅有使用map的缓存区
///
/// Copyright (c) 2013 广州致远电子股份有限公司软件部
///
/// 作者：
///    xuchaoze  2013.2.4
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///	   ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                         |
///
#include "stdafx.h"
#include "sclcache.h"
#include "sclcachewithmap.h"

///
/// @brief
///     SCLCache的接口对象实例
///
/// @return
///     SCLCache实例
///
SCLCache *CreateSCLCache(SCL_CACHE_TYPE type)
{
	switch(type)
	{
	case CACHE_WITH_MAP:
		{
			return new SCLCacheWithMap;
		}
		break;
	default:
		{
			return new SCLCacheWithMap;
		}
	}
}

///
/// @brief
///     释放SCLCache对象实例
///
/// @param[in]
///      sclCache - 指向接口对象实例的指针
///
/// @return
///     
///
void ReleaseSCLCache(SCLCache *sclCache)
{
	if ( sclCache != NULL )
	{
		delete sclCache;
	}
}