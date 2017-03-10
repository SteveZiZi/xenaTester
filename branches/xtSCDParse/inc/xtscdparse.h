#ifndef SCDPARSE_H
#define SCDPARSE_H

#include "xtscdparse_global.h"


class CSCDParsePrivate;
class XTSCDPARSE_EXPORT CSCDParse
{
public:
    CSCDParse();
    ~CSCDParse();

    /*
     *  @Func:  SCD_Load
     *  
     *  @Param[in]:
     *      fileName:   SCD文件加载
     *  
     *  @Param[out]:
     *      None
     *  
     *  @Return:
     *      true:       加载成功
     *      false:      加载失败
     */
    bool load(char* fileName);

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
    void unload(void);

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
     *      获取SCD文件中IED设备的个数
     */
    int iedNum(void);

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
     *      获取指定IED设备的信息
     */
//    bool getIEDInfo(int iedItem, IED_INFO_t* ied);

private:
    CSCDParsePrivate* m_data;
};


#endif // SCDPARSE_H
