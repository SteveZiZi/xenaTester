#include "include/ISCLCacheManager.h"
#include "sclcachemanager.h"

ISCLCacheManager::ISCLCacheManager()
{

}


ISCLCacheManager::~ISCLCacheManager()
{

}

///
/// @brief
///     SCL�ļ���������ʵ��
///
/// @return
///     SCL�ļ���������ʵ��ָ��
///
SCLMANAGER_API ISCLCacheManager *CreateSCLCacheManagerImpl( void )
{
	return new SCLCacheManager;
}

///
/// @brief
///     �ͷ�SCL�ļ���������ʵ��
///
/// @param[in]
///      objPtr - ָ��SCL�ļ���������ʵ����ָ��
///
/// @return
///     
///
SCLMANAGER_API void  RelaseSCLCacheManagerImpl(ISCLCacheManager *objPtr)
{
	if ( objPtr != NULL )
	{
		delete objPtr;
		objPtr = NULL;
	}
}
