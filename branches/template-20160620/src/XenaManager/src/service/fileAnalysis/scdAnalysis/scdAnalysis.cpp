#include "scdAnalysis.h"
#include <QStringList>
#include <QDebug>

#include <atltime.h>
#include "MessageCodec/include/sav92_msg.h"
#include "MessageCodec/include/goose_msg.h"

static bool fillGooseData(enumChannelType type, GOOSE_DATA_VARIANT* data);

#define __safe_delete_arr(arr) \
    do{if(arr) {delete[] arr; arr=NULL;}}while(0)

#define QString_to_MAC(str, mac)\
    do {                                                    \
        QStringList macList = str.split(QChar('-'));        \
        memset(mac, 0, MAC_ADDR_LENTH);                \
        int i = 0;                                          \
        Q_FOREACH(QString str1, macList) {                   \
            if (i < MAC_ADDR_LENTH)              \
                mac[i++]=str1.toUInt(0, 16);             \
        }                                                   \
    } while( 0 )

CScdAnalysis::CScdAnalysis()
{
    m_sclMgr = CSCLManager::getInstance();
    m_gseAnalysis = m_sclMgr->GetGooseAnalysis();
    m_smvAnalysis = m_sclMgr->GetSmv92Analysis();
    m_gseIedInfo = NULL;
    m_gseIedNum = 0;
    m_smvIedInfo = NULL;
    m_smvIedNum = 0;

    m_srcMacAddr[0] =  0x00;
    m_srcMacAddr[1] =  0x00;
    m_srcMacAddr[2] =  0xFF;
    m_srcMacAddr[3] =  0xFF;
    m_srcMacAddr[4] =  0xFF;
    m_srcMacAddr[5] =  0xFF;
}

CScdAnalysis::~CScdAnalysis()
{
    __safe_delete_arr(m_gseIedInfo);
    __safe_delete_arr(m_smvIedInfo);
}

CScdAnalysis* CScdAnalysis::getInstance()
{
    static CScdAnalysis anaylsis;

    return &anaylsis;
}


void CScdAnalysis::setSrcMacAddr(unsigned char* mac, int length)
{
    if (length == MAC_ADDR_LENTH) {
        memcpy(m_srcMacAddr, mac, MAC_ADDR_LENTH);
    }
}

void CScdAnalysis::setSrcMacAddr(const QString &mac)
{
    QString_to_MAC(mac, m_srcMacAddr);
}

bool CScdAnalysis::parseFile(const QString& fileName)
{
    m_sclMgr = CSCLManager::getInstance();
    if (m_sclMgr->Load(fileName.toStdWString())) {
        qDebug() << "load" << fileName << "failed.";
        return false;
    }

    m_smvIedNum = m_smvAnalysis->GetIEDSMVNum();
    if (m_smvIedNum) {
        __safe_delete_arr(m_smvIedInfo);
        m_smvIedInfo = new SCDSMV92ANALYSIS_IEDSMV_INFO[m_smvIedNum];
        m_smvAnalysis->GetAllIEDSMVInfo(m_smvIedInfo, m_smvIedNum);
    }

    m_gseIedNum = m_gseAnalysis->GetIEDGOOSENum();
    if (m_gseAnalysis) {
        __safe_delete_arr(m_gseIedInfo);
        m_gseIedInfo = new SCDGOOSEANALYSIS_IEDGOOSE_INFO[m_gseIedNum];
        m_gseAnalysis->GetAllIEDGOOSEInfo(m_gseIedInfo, m_gseIedNum);
    }

    return true;
}

static bool macAddrEquare(unsigned char* mac1, unsigned char* mac2, int len)
{
    for(int i = 0; i < len; i++) {
        if (mac1[i] != mac2[i])
            return false;
    }
    return true;
}

const SCDSMV92ANALYSIS_IEDSMV_INFO* CScdAnalysis::smvIedInfo(const QString& mac, quint16 appId)
{
    unsigned char macAddr[MAC_ADDR_LENTH];
    QString_to_MAC(mac, macAddr);

    for(int i = 0; i < m_smvIedNum; i++) {
        if (macAddrEquare(m_smvIedInfo[i].smvMAC, macAddr, MAC_ADDR_LENTH) && 
            m_smvIedInfo[i].smvAppID == appId)
        {
            return m_smvIedInfo+i;
        }
    }
    return NULL;
}

const SCDGOOSEANALYSIS_IEDGOOSE_INFO* CScdAnalysis::gseIedInfo(const QString& mac, quint16 appId)
{
    unsigned char macAddr[MAC_ADDR_LENTH];
    QString_to_MAC(mac, macAddr);

    for(int i = 0; i < m_gseIedNum; i++) {
        if (macAddrEquare(m_gseIedInfo[i].gooseMAC, macAddr, MAC_ADDR_LENTH) && 
            m_gseIedInfo[i].gooseAppID == appId)
        {
            return m_gseIedInfo+i;
        }
    }
    return NULL;
}

///
/// @brief
///     获取指定索引地址的SMV控制块所控制的DataSet的所有通道（FCDA）信息
///
/// @param[in]
///     iedName        - SMV控制块所在的IED的名称
/// @param[in]
///     ldInst         - SMV控制块所在的LDevice的Inst号
/// @param[in]
///     cbName         - SMV控制块的名称
/// @param[in]
///     dataSetInfoArr - 保存SMV的通道信息的数组
/// @param[in]
///     arrSize        - 数组的最大容量
///
/// @return
///     数组保存通道信息的数量：操作成功；  -1：失败
///
int CScdAnalysis::smvChnlInfo(const std::wstring &iedName, 
                              const std::wstring &ldInst,
                              const std::wstring &cbName, 
                              SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
                              const int arrSize)
{
    return m_sclMgr->GetAllIEDSMVChannelInfo(iedName, ldInst, cbName, dataSetInfoArr, arrSize);
}

///
/// @brief
///     获取指定索引地址的GOOSE控制块所控制的DataSet的所有通道（FCDA）信息
///
/// @param[in]
///     iedName        - GOOSE控制块所在的IED的名称
/// @param[in]
///     ldInst         - GOOSE控制块所在的LDevice的Inst号
/// @param[in]
///     cbName         - GOOSE控制块的名称
/// @param[in]
///     dataSetInfoArr - 保存GOOSE的通道信息的数组
/// @param[in]
///     arrSize        - 数组的最大容量
///
/// @return
///     数组保存通道信息的数量：操作成功；  -1：失败
///
int CScdAnalysis::gseChnlInfo(const std::wstring &iedName,
                              const std::wstring &ldInst,
                              const std::wstring &cbName,
                              SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
                              const int arrSize)
{
    return m_sclMgr->GetAllIEDGOOSEChannelInfo(iedName, ldInst, cbName, dataSetInfoArr, arrSize);
}

///
/// @brief
///     获取指定索引地址的GOOSE控制块的控制的某一父通道的子通道的数目
///
/// @param[in]
///     iedName        - 该子通道所在的IED的名称
/// @param[in]
///     ldInst         - 该子通道所在的LDevice的Inst号
// @param[in]
///     dataSetInfo    - 父通道信息
///
/// @return
///     子通道数目 - 操作成功； -1 - 操作失败
///
int CScdAnalysis::gseOneChnlSubChnlNum(
                                     const std::wstring &iedName,
                                     const std::wstring &ldInst,
                                     const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo)
{
    return m_sclMgr->GetIEDGOOSESubChannelNum(iedName, ldInst, dataSetInfo);
}

///
/// @brief
///     获取指定索引地址的GOOSE控制块的控制的某一父通道的全部子通道的信息
///
/// @note
///     SCL文件中某通道信息定义出错则该通道信息不予返回
///
/// @param[in]
///     iedName           - 该子通道所在的IED的名称
/// @param[in]
///     ldInst            - 该子通道所在的LDevice的Inst号
// @param[in]
///     dataSetInfo       - 父通道信息
// @param[in]
///     subChannelInfoArr - 保存子通道信息的数组
// @param[in]
///     arrSize           - 数组大小
///
/// @return
///     实际的子通道数 - 操作成功； -1 - 操作失败
///
int CScdAnalysis::gseOneChnlSubChnlInfo(
                                      const std::wstring &iedName,
                                      const std::wstring &ldInst,
                                      const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
                                      SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
                                      const int arrSize )
{
    return m_sclMgr->GetAllIEDGOOSESubChannelInfo(iedName, ldInst, dataSetInfo, subChannelInfoArr, arrSize);
}


///
/// @brief
///     根据MAC地址、appId生产对应的SMV报文
///
/// @param[in]
///     mac               - 该SMV控制块的MAC地址字符串，格式 xx-xx-xx-xx-xx-xx
/// @param[in]
///     appId             - 该SMV控制块的APPID号
// @param[in]
///     data              - 存放报文的数据缓冲区
// @param[in]
///     size              - data的缓冲区大小
///
/// @return
///     -1：              - 生成报文出现错误，可能是没有找到对应的SMV控制块
///     >0：              - 生产报文成功，并返回报文的有效长度
///
int CScdAnalysis::smvGenerateFrame(const QString& mac, quint16 appId, quint8 *data, quint32 size)
{
    const SCDSMV92ANALYSIS_IEDSMV_INFO *smvCB = this->smvIedInfo(mac, appId);
    if (smvCB == NULL || data == NULL) {
        return 0;
    }

    int iByteCount = 0;
    unsigned char macAddr[MAC_ADDR_LENTH];
    QString_to_MAC(mac, macAddr);

    Sav92Msg * sav92Msg = CreateSav92MsgImpl();

    sav92Msg->ClearSavPDU();
    sav92Msg->Set8021QVlanInfo(smvCB->smvVlanPri, smvCB->smvVlanID);
    sav92Msg->SetAppID(appId);
    sav92Msg->SetMacAddr(macAddr, m_srcMacAddr, MAC_ADDR_LENTH);
    SAV92_SAVPDU_OPTS opts;
    opts.bDataSetOpt = smvCB->smvCbOptDataRef;
    opts.bRefrTmOpt = smvCB->smvCbOptRefreshTime;
    opts.bSecurityOpt = false/*smvCB->smvCbOptSecurity*/;
    opts.bSmpRateOpt = smvCB->smvCbOptSmpRate;
    sav92Msg->SetSavPduOpts(opts);

    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *channelInfoArr = new SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO[smvCB->channelNum];
    int chnlNum = this->smvChnlInfo(smvCB->iedName, smvCB->iedLdInst, smvCB->smvCbName,channelInfoArr,smvCB->channelNum);

    //Add Asdu
    uint32_t *chnlVal = new uint32_t[chnlNum];
    memset(chnlVal, 0, sizeof(uint32_t)*chnlNum);
    int asduNum = smvCB->smvCbNofASDU;
    if (asduNum > 8) {
        qDebug() << "CScdAnalysis::smvGenerateFrame -> Asdu Num is Erorr!!!";
        asduNum = 8;
    }
    SAV92_ASDU_PUBLIC asduPublic;
    std::string         strText;
    for(int i = 0; i < asduNum; i++) {
        std::wstring dataSet = smvCB->iedName + smvCB->iedLdInst + smvCB->dataSetName;
        strText.assign(dataSet.begin(), dataSet.end());
        int len = qMin<int>(SAV92_VISIBLE_STR_MAX_SIZE, strText.length());
        memcpy_s(asduPublic.datSetStr, SAV92_VISIBLE_STR_MAX_SIZE, strText.c_str(), len);
        asduPublic.datSetStr[len] = '\0';

        strText.assign(smvCB->smvCbSmvID.begin(), smvCB->smvCbSmvID.end());
        len = qMin<int>(SAV92_VISIBLE_STR_MAX_SIZE, strText.length());
        memcpy_s(asduPublic.svIDStr, SAV92_VISIBLE_STR_MAX_SIZE, strText.c_str(), len);
        asduPublic.svIDStr[len] = '\0';

        asduPublic.smpCnt = i;
        asduPublic.confRev = smvCB->smvCbConfRev;
        asduPublic.refrTm.ts_s = 0;
        asduPublic.refrTm.ts_ns = 0;
        asduPublic.refrTm.ts_q = 0;
        asduPublic.smpSynch = true;
        asduPublic.smpRate = smvCB->smvCbSmpRate;
        if (!sav92Msg->AddAnASDU(asduPublic, chnlVal, chnlVal, chnlNum)) {
            goto exitCode;
        }
    }

    //
    // 根据设置的参数进行编码，获得报文帧
    //
    iByteCount = (int)(sav92Msg->Code(data, size));
    if (0 == iByteCount)    ///< 检查帧大小
    {
        qDebug() << "CScdAnalysis::smvGenerateFrame -> Code frame failed";
        goto exitCode;
    }

#ifdef _DEBUG
    if (0) {
        int pos = 0;
        QString str;
        for(int i = 0; i < iByteCount/8; i++) {
            str = QString("%1 %2 %3 %4  %5 %6 %7 %8")
                .arg(data[pos],   2, 16, QChar('0'))
                .arg(data[pos+1], 2, 16, QChar('0'))
                .arg(data[pos+2], 2, 16, QChar('0'))
                .arg(data[pos+3], 2, 16, QChar('0'))
                .arg(data[pos+4], 2, 16, QChar('0'))
                .arg(data[pos+5], 2, 16, QChar('0'))
                .arg(data[pos+6], 2, 16, QChar('0'))
                .arg(data[pos+7], 2, 16, QChar('0'));
            pos += 8;
            qDebug() << str;
        }
        str = "";
        for (; pos < iByteCount; pos++)
        {
            str.append(QString("%1 ").arg(data[pos], 2, 16, QChar('0')));
        }
        qDebug() << str;
    }
#endif

exitCode:
    ReleaseSav92MsgImpl(sav92Msg);
    delete[] chnlVal;
    delete[] channelInfoArr;
    return iByteCount;
}

///
/// @brief
///     根据MAC地址、appId生产对应的GOOSE报文
///
/// @param[in]
///     mac               - 该GOOSE控制块的MAC地址字符串，格式 xx-xx-xx-xx-xx-xx
/// @param[in]
///     appId             - 该GOOSE控制块的APPID号
// @param[in]
///     data              - 存放报文的数据缓冲区
// @param[in]
///     size              - data的缓冲区大小
///
/// @return
///     -1：              - 生成报文出现错误，可能是没有找到对应的SMV控制块
///     >0：              - 生产报文成功，并返回报文的有效长度
///
int CScdAnalysis::gseGenerateFrame(const QString& mac, quint16 appId, quint8 *data, quint32 size)
{
    const SCDGOOSEANALYSIS_IEDGOOSE_INFO *cb = this->gseIedInfo(mac, appId);
    if (cb == NULL || data == NULL) {
        return 0;
    }

    int iByteCount = 0;
    unsigned char macAddr[MAC_ADDR_LENTH];
    QString_to_MAC(mac, macAddr);

    GooseMsg * msg = CreateGooseMsgImpl();
    msg->ClearGoosePDU();
    msg->Set8021QVlanInfo(cb->gooseVlanPri, cb->gooseVlanID);
    msg->SetAppID(appId);
    msg->SetMacAddr(macAddr, m_srcMacAddr, MAC_ADDR_LENTH);

    SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO  *channelInfoArr = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO [cb->channelNum];
    int chnlNum = this->gseChnlInfo(cb->iedName, cb->iedLdInst, cb->cbName,channelInfoArr,cb->channelNum);

    GOOSE_PDU_PUBLIC asduPublic;
    // 控制块引用
    std::wstring strValue = cb->iedName + cb->iedLdInst + L"/LLN0$GO$" + cb->cbName;
    asduPublic.gocbRef.assign(strValue.begin(), strValue.end());

    // 数据集的引用
    strValue = cb->iedName + cb->iedLdInst + L"/LLN0$" + cb->dataSet;
    asduPublic.datSet.assign(strValue.begin(), strValue.end());

    // 报文的唯一性标识
    strValue = cb->gooseID;
    asduPublic.goID.assign(strValue.begin(), strValue.end());

    // 报文加密
    strValue = L"";
    asduPublic.security.assign(strValue.begin(), strValue.end());

    // 其他配置信息
    asduPublic.timeAllowedtoLive   = 2 * cb->gooseMaxTime;
    asduPublic.eventTm.ts_s        = 0;                            ///< 时间戳的秒数部分
    asduPublic.eventTm.ts_ns       = 0;                            ///< 时间戳的纳秒数部分
    asduPublic.eventTm.ts_q        = 0;                            ///< 时间戳的品质
    asduPublic.stNum               = 0;                  ;         ///< 状态序号（变位次数）
    asduPublic.sqNum               = 0;                            ///< 顺序号
    asduPublic.test                = 0;
    asduPublic.confRev             = cb->confRev;                   ///< 配制版本号
    asduPublic.ndsCom              = 0;
    asduPublic.numDatSetEntries    = chnlNum;                       ///< 通道数
    msg->SetPduPublicInfo(asduPublic);

    // 遍历所有通道
    for (int chnl = 0; chnl < chnlNum; chnl++)
    {
        int subChnlNum = this->gseOneChnlSubChnlNum(cb->iedName, cb->iedLdInst, channelInfoArr[chnl]);
        if (0 == subChnlNum) {
            // 获得通道值
            GOOSE_DATA_VARIANT stChannelValue;
            if (!fillGooseData(this->__GetChannelType(channelInfoArr[chnl]), &stChannelValue)) {
                goto exitCode;
            }
            // 向帧管理器添加一个数据通道
            if (!msg->AddAChannel(&(stChannelValue.m_stUnKnown.id))) {
                goto exitCode;
            }
        }
        else {
            // 创建结构体类型通道对象
            GOOSE_STRUCT stGooseStruct;
            stGooseStruct.id.type   = GSEDT_STRUCT;
            stGooseStruct.member.clear();

            SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO * subChnlInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[subChnlNum];
            if (-1 == this->gseOneChnlSubChnlInfo(cb->iedName, cb->iedLdInst, channelInfoArr[chnl], subChnlInfo, subChnlNum)) {
                delete[] subChnlInfo;
                goto exitCode;
            }
            for(int i = 0; i < subChnlNum; i++) {
                // 向结构体对象添加子通道值对象指针
                GOOSE_DATA_VARIANT * gseData = new GOOSE_DATA_VARIANT();
                if (!fillGooseData(this->__GetChannelType(subChnlInfo[i]), gseData)) {
                    for(std::vector<GOOSE_DATA *>::iterator it = stGooseStruct.member.begin(); it != stGooseStruct.member.end(); it++) {
                        delete (*it);
                    }
                    delete[] subChnlInfo;
                    goto exitCode;
                }
                stGooseStruct.member.push_back(&(gseData->m_stUnKnown.id));
            }

            // 向帧管理器添加一个数据通道
            int ret = msg->AddAChannel(&(stGooseStruct.id));
            for(std::vector<GOOSE_DATA *>::iterator it = stGooseStruct.member.begin(); it != stGooseStruct.member.end(); it++) {
                delete (*it);
            }
            delete[] subChnlInfo;
            if (ret == -1) {
                goto exitCode;
            }
        }
    }

    //
    // 根据设置的参数进行编码，获得报文帧
    //
    iByteCount = (int)(msg->Code(data, size));
    if (0 == iByteCount)                                        ///< 检查帧大小
    {
        qDebug() << "CScdAnalysis::smvGenerateFrame -> Code frame failed";
        goto exitCode;
    }

#ifdef _DEBUG
    if (0) {
        int pos = 0;
        QString str;
        for(int i = 0; i < iByteCount/8; i++) {
            str = QString("%1 %2 %3 %4  %5 %6 %7 %8")
                .arg(data[pos],   2, 16, QChar('0'))
                .arg(data[pos+1], 2, 16, QChar('0'))
                .arg(data[pos+2], 2, 16, QChar('0'))
                .arg(data[pos+3], 2, 16, QChar('0'))
                .arg(data[pos+4], 2, 16, QChar('0'))
                .arg(data[pos+5], 2, 16, QChar('0'))
                .arg(data[pos+6], 2, 16, QChar('0'))
                .arg(data[pos+7], 2, 16, QChar('0'));
            pos += 8;
            qDebug() << str;
        }
        str = "";
        for (; pos < iByteCount; pos++)
        {
            str.append(QString("%1 ").arg(data[pos], 2, 16, QChar('0')));
        }
        qDebug() << str;
    }
#endif

exitCode:
    ReleaseGooseMsgImpl(msg);
    delete[] channelInfoArr;
    return iByteCount;
}


// 相别信息判断模板
static std::wstring PHASE_A_TEMPLATE[]={L"A相", L"a相", L"Ａ相", L"ａ相"};
static std::wstring PHASE_B_TEMPLATE[]={L"B相", L"b相", L"Ｂ相", L"ｂ相"};
static std::wstring PHASE_C_TEMPLATE[]={L"C相", L"c相", L"Ｃ相", L"ｃ相"};
static std::wstring PHASE_X_TEMPLATE[]={L"X相", L"x相", L"同期", L"母线", L"抽取", L"Ｘ相", L"ｘ相"};
static std::wstring PHASE_N_TEMPLATE[]={L"N相", L"n相", L"中性点", L"Ｎ相", L"ｎ相"};
static std::wstring TIME_92_TEMPLATE[]={L"延时", L"时间"};
///
/// @brief
///    获取通道的类型信息
///
/// @param[in] stSmvChannelInfo 采样值数据集中的数据通道信息
///
/// @return
///     返回对应的通道类型信息 时间/电压/电流...
///
enumChannelType CScdAnalysis::__GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo )
{
    enumChannelType enChannelType = CHANNEL_TYPE_UNKOWN;
    int nSize = sizeof(TIME_92_TEMPLATE)/sizeof(TIME_92_TEMPLATE[0]);
    for (int i = 0; i < nSize; ++i)
    {
        if (stSmvChannelInfo.doDesc.find(TIME_92_TEMPLATE[i].c_str()) != std::wstring::npos)
        {
            return CHANNEL_TYPE_SMV_TIME;
        }
    }

    if (stSmvChannelInfo.doName == L"DelayTRtg")
    {
        enChannelType = CHANNEL_TYPE_SMV_TIME;
    }
    else if (stSmvChannelInfo.lnClass == L"TVTR")
    {
        enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
    }
    else if (stSmvChannelInfo.lnClass == L"TCTR")
    {
        enChannelType = CHANNEL_TYPE_SMV_CURRENT;
    }
    else
    {
        if (stSmvChannelInfo.daVal.find(L"电压") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.daVal.find(L"电流") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.doDesc.find(L"电压") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.doDesc.find(L"电流") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.lnDesc.find(L"电压") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }                                           
        else if (stSmvChannelInfo.lnDesc.find(L"电流") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
    }
    return enChannelType;
}

///
/// @brief
///    获取通道的类型信息
///
/// @param[in] stGooseChannelInfo goose数据集中的数据通道信息
///
/// @return
///     返回对应的通道类型信息 单电/双点/时间...
///
enumChannelType CScdAnalysis::__GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo )
{
    enumChannelType enChannelType = CHANNEL_TYPE_UNKOWN;
    if (stGooseChannelInfo.daBType == L"BOOLEAN")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_POINT;
    }
    else if (stGooseChannelInfo.daBType == L"Quality")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_QUALITY;
    }
    else if (stGooseChannelInfo.daBType == L"Timestamp")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_TIME;
    }
    else if (stGooseChannelInfo.daBType == L"Dbpos")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_DOUBLE_POINT;
    }
    else if (stGooseChannelInfo.daBType == L"FLOAT32")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_FLOAT;
    }
    else if (stGooseChannelInfo.daBType == L"INT8")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT8;
    }
    else if (stGooseChannelInfo.daBType == L"INT16")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT16;
    }
    else if (stGooseChannelInfo.daBType == L"INT24")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT24;
    }
    else if (stGooseChannelInfo.daBType == L"INT32")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_INT32;
    }
    else if (stGooseChannelInfo.daBType == L"INT8U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT8;
    }
    else if (stGooseChannelInfo.daBType == L"INT16U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT16;
    }
    else if (stGooseChannelInfo.daBType == L"INT24U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT24;
    }
    else if (stGooseChannelInfo.daBType == L"INT32U")
    {
        enChannelType = CHANNEL_TYPE_GOOSE_UINT32;
    }
    else if ((stGooseChannelInfo.daBType.empty() && !stGooseChannelInfo.doName.empty()) || (stGooseChannelInfo.daBType == L"Struct"))
    {
        enChannelType = CHANNEL_TYPE_GOOSE_STRUCT;
    }
    else
    {
        enChannelType = CHANNEL_TYPE_UNKOWN;
    }

    return enChannelType;
}

static bool fillGooseData(enumChannelType type, GOOSE_DATA_VARIANT* data)
{
    switch(type) {
    case CHANNEL_TYPE_GOOSE_POINT:
        data->__SetType(GSEDT_BOOLEAN);
        data->m_stBoolean.boolVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_QUALITY:
    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        data->__SetType(GSEDT_BIT_STR);
        data->m_stBitString.SetString(strDPValue[DP_VALUE_INIT]);
        break;
    case CHANNEL_TYPE_GOOSE_TIME:
        data->__SetType(GSEDT_UTC_TIME);
        break;
    case CHANNEL_TYPE_GOOSE_FLOAT:
        data->__SetType(GSEDT_FLOAT);
        data->m_stFloat.datVal = 0.0f;
        break;
    case CHANNEL_TYPE_GOOSE_INT8:
        data->__SetType(GSEDT_INT8);
        data->m_stInt8.datVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_INT16:
        data->__SetType(GSEDT_INT16);
        data->m_stInt16.datVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_INT24:
        data->__SetType(GSEDT_INT24);
        data->m_stInt24.datVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_INT32:
        data->__SetType(GSEDT_INT32);
        data->m_stInt32.datVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_UINT8:
        data->__SetType(GSEDT_UINT8);
        data->m_stUInt8.datVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_UINT16:
        data->__SetType(GSEDT_UINT16);
        data->m_stUInt16.datVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_UINT24:
        data->__SetType(GSEDT_UINT24);
        data->m_stUInt24.datVal = 0;
        break;
    case CHANNEL_TYPE_GOOSE_UINT32:
        data->__SetType(GSEDT_UINT32);
        data->m_stUInt32.datVal = 0;
        break;
    default:
        return false;
        break;
    }
    return true;
}