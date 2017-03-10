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
     *      fileName:   SCD�ļ�����
     *  
     *  @Param[out]:
     *      None
     *  
     *  @Return:
     *      true:       ���سɹ�
     *      false:      ����ʧ��
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
     *      ��ȡSCD�ļ���IED�豸�ĸ���
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
     *      ��ȡָ��IED�豸����Ϣ
     */
//    bool getIEDInfo(int iedItem, IED_INFO_t* ied);

private:
    CSCDParsePrivate* m_data;
};


#endif // SCDPARSE_H
