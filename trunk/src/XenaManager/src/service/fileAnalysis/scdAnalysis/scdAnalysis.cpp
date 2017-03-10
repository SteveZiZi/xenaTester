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
///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
///
/// @param[in]
///     iedName        - SMV���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - SMV���ƿ������
/// @param[in]
///     dataSetInfoArr - ����SMV��ͨ����Ϣ������
/// @param[in]
///     arrSize        - ������������
///
/// @return
///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
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
///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
///
/// @param[in]
///     iedName        - GOOSE���ƿ����ڵ�IED������
/// @param[in]
///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
/// @param[in]
///     cbName         - GOOSE���ƿ������
/// @param[in]
///     dataSetInfoArr - ����GOOSE��ͨ����Ϣ������
/// @param[in]
///     arrSize        - ������������
///
/// @return
///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
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
///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ������ͨ������Ŀ
///
/// @param[in]
///     iedName        - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst         - ����ͨ�����ڵ�LDevice��Inst��
// @param[in]
///     dataSetInfo    - ��ͨ����Ϣ
///
/// @return
///     ��ͨ����Ŀ - �����ɹ��� -1 - ����ʧ��
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
///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ȫ����ͨ������Ϣ
///
/// @note
///     SCL�ļ���ĳͨ����Ϣ����������ͨ����Ϣ���践��
///
/// @param[in]
///     iedName           - ����ͨ�����ڵ�IED������
/// @param[in]
///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
// @param[in]
///     dataSetInfo       - ��ͨ����Ϣ
// @param[in]
///     subChannelInfoArr - ������ͨ����Ϣ������
// @param[in]
///     arrSize           - �����С
///
/// @return
///     ʵ�ʵ���ͨ���� - �����ɹ��� -1 - ����ʧ��
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
///     ����MAC��ַ��appId������Ӧ��SMV����
///
/// @param[in]
///     mac               - ��SMV���ƿ��MAC��ַ�ַ�������ʽ xx-xx-xx-xx-xx-xx
/// @param[in]
///     appId             - ��SMV���ƿ��APPID��
// @param[in]
///     data              - ��ű��ĵ����ݻ�����
// @param[in]
///     size              - data�Ļ�������С
///
/// @return
///     -1��              - ���ɱ��ĳ��ִ��󣬿�����û���ҵ���Ӧ��SMV���ƿ�
///     >0��              - �������ĳɹ��������ر��ĵ���Ч����
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
    // �������õĲ������б��룬��ñ���֡
    //
    iByteCount = (int)(sav92Msg->Code(data, size));
    if (0 == iByteCount)    ///< ���֡��С
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
///     ����MAC��ַ��appId������Ӧ��GOOSE����
///
/// @param[in]
///     mac               - ��GOOSE���ƿ��MAC��ַ�ַ�������ʽ xx-xx-xx-xx-xx-xx
/// @param[in]
///     appId             - ��GOOSE���ƿ��APPID��
// @param[in]
///     data              - ��ű��ĵ����ݻ�����
// @param[in]
///     size              - data�Ļ�������С
///
/// @return
///     -1��              - ���ɱ��ĳ��ִ��󣬿�����û���ҵ���Ӧ��SMV���ƿ�
///     >0��              - �������ĳɹ��������ر��ĵ���Ч����
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
    // ���ƿ�����
    std::wstring strValue = cb->iedName + cb->iedLdInst + L"/LLN0$GO$" + cb->cbName;
    asduPublic.gocbRef.assign(strValue.begin(), strValue.end());

    // ���ݼ�������
    strValue = cb->iedName + cb->iedLdInst + L"/LLN0$" + cb->dataSet;
    asduPublic.datSet.assign(strValue.begin(), strValue.end());

    // ���ĵ�Ψһ�Ա�ʶ
    strValue = cb->gooseID;
    asduPublic.goID.assign(strValue.begin(), strValue.end());

    // ���ļ���
    strValue = L"";
    asduPublic.security.assign(strValue.begin(), strValue.end());

    // ����������Ϣ
    asduPublic.timeAllowedtoLive   = 2 * cb->gooseMaxTime;
    asduPublic.eventTm.ts_s        = 0;                            ///< ʱ�������������
    asduPublic.eventTm.ts_ns       = 0;                            ///< ʱ���������������
    asduPublic.eventTm.ts_q        = 0;                            ///< ʱ�����Ʒ��
    asduPublic.stNum               = 0;                  ;         ///< ״̬��ţ���λ������
    asduPublic.sqNum               = 0;                            ///< ˳���
    asduPublic.test                = 0;
    asduPublic.confRev             = cb->confRev;                   ///< ���ư汾��
    asduPublic.ndsCom              = 0;
    asduPublic.numDatSetEntries    = chnlNum;                       ///< ͨ����
    msg->SetPduPublicInfo(asduPublic);

    // ��������ͨ��
    for (int chnl = 0; chnl < chnlNum; chnl++)
    {
        int subChnlNum = this->gseOneChnlSubChnlNum(cb->iedName, cb->iedLdInst, channelInfoArr[chnl]);
        if (0 == subChnlNum) {
            // ���ͨ��ֵ
            GOOSE_DATA_VARIANT stChannelValue;
            if (!fillGooseData(this->__GetChannelType(channelInfoArr[chnl]), &stChannelValue)) {
                goto exitCode;
            }
            // ��֡���������һ������ͨ��
            if (!msg->AddAChannel(&(stChannelValue.m_stUnKnown.id))) {
                goto exitCode;
            }
        }
        else {
            // �����ṹ������ͨ������
            GOOSE_STRUCT stGooseStruct;
            stGooseStruct.id.type   = GSEDT_STRUCT;
            stGooseStruct.member.clear();

            SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO * subChnlInfo = new SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO[subChnlNum];
            if (-1 == this->gseOneChnlSubChnlInfo(cb->iedName, cb->iedLdInst, channelInfoArr[chnl], subChnlInfo, subChnlNum)) {
                delete[] subChnlInfo;
                goto exitCode;
            }
            for(int i = 0; i < subChnlNum; i++) {
                // ��ṹ����������ͨ��ֵ����ָ��
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

            // ��֡���������һ������ͨ��
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
    // �������õĲ������б��룬��ñ���֡
    //
    iByteCount = (int)(msg->Code(data, size));
    if (0 == iByteCount)                                        ///< ���֡��С
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


// �����Ϣ�ж�ģ��
static std::wstring PHASE_A_TEMPLATE[]={L"A��", L"a��", L"����", L"����"};
static std::wstring PHASE_B_TEMPLATE[]={L"B��", L"b��", L"����", L"����"};
static std::wstring PHASE_C_TEMPLATE[]={L"C��", L"c��", L"����", L"����"};
static std::wstring PHASE_X_TEMPLATE[]={L"X��", L"x��", L"ͬ��", L"ĸ��", L"��ȡ", L"����", L"����"};
static std::wstring PHASE_N_TEMPLATE[]={L"N��", L"n��", L"���Ե�", L"����", L"����"};
static std::wstring TIME_92_TEMPLATE[]={L"��ʱ", L"ʱ��"};
///
/// @brief
///    ��ȡͨ����������Ϣ
///
/// @param[in] stSmvChannelInfo ����ֵ���ݼ��е�����ͨ����Ϣ
///
/// @return
///     ���ض�Ӧ��ͨ��������Ϣ ʱ��/��ѹ/����...
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
        if (stSmvChannelInfo.daVal.find(L"��ѹ") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.daVal.find(L"����") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.doDesc.find(L"��ѹ") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }
        else if (stSmvChannelInfo.doDesc.find(L"����") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
        else if (stSmvChannelInfo.lnDesc.find(L"��ѹ") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_VOLTAGE;
        }                                           
        else if (stSmvChannelInfo.lnDesc.find(L"����") != std::wstring::npos)
        {
            enChannelType = CHANNEL_TYPE_SMV_CURRENT;
        }
    }
    return enChannelType;
}

///
/// @brief
///    ��ȡͨ����������Ϣ
///
/// @param[in] stGooseChannelInfo goose���ݼ��е�����ͨ����Ϣ
///
/// @return
///     ���ض�Ӧ��ͨ��������Ϣ ����/˫��/ʱ��...
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