#include "../inc/xtscdparse.h"

#include "sclParse.h"

class CSCDParsePrivate {
public:

    CSclParse m_sclParse;

    friend class CSCDParse;
};


CSCDParse::CSCDParse()
: m_data(new CSCDParsePrivate())
{

}

CSCDParse::~CSCDParse()
{
    delete m_data;
    m_data = NULL;
}

/*
 *  @Func:  SCD_Load
 *  
 *  @Param[in]:
 *      fileName:   SCD文件名，包含路径
 *  
 *  @Param[out]:
 *      None
 *  
 *  @Return:
 *      true:       加载成功
 *      false:      加载成功
 */
bool CSCDParse::load(char* fileName)
{
    m_data->m_sclParse.load(QString::fromLocal8Bit(fileName));
    return true;
}

/*
 *  @Func:  SCD_Unload
 *  
 *  @Param[in]:
 *      None
 *  
 *  @Param[out]:
 *      None
 *  
 *  @Return:
 *      None
 */
void CSCDParse::unload(void)
{

}

/*
 *  @Func:  SCD_IEDNum
 *  
 *  @Param[in]:
 *      None
 *  
 *  @Param[out]:
 *      None
 *  
 *  @Return:
 *      返回SCD文件中IED设备的个数
 */
int CSCDParse::IEDNum(void)
{
    return -1;
}

/*
 *  @Func:  SCD_GetIEDInfo
 *  
 *  @Param[in]:
 *      None
 *  
 *  @Param[out]:
 *      None
 *  
 *  @Return:
 *      返回SCD文件中IED设备的个数
 */
bool CSCDParse::getIEDInfo(int iedItem, IED_INFO_t* ied)
{
    return false;
}
