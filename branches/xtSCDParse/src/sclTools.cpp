#include "sclTools.h"

#include <QFile>
#include <QXmlStreamReader>

#include "debug.h"

SCLTools::SCLTools()
: m_reader(NULL)
, m_file(NULL)
, m_hasLoad(false)
{

}

SCLTools::~SCLTools()
{
    unload();
}


bool SCLTools::load(const SCL_STR &fileName)
{
    if (m_hasLoad) {
        return true;
    }

    m_file = new QFile(fileName.c_str());
    if(!m_file->open(QFile::ReadOnly | QFile::Text)) {
        SCL_ERROR("load open %s fail.", fileName);
        return false;  
    }  
    m_reader = new QXmlStreamReader(m_file);

    return true;
}

void SCLTools::unload()
{
    if (m_hasLoad) {
        delete m_reader;
        m_reader = NULL;
        m_file->close();
        delete m_file;
        m_file = NULL;
        m_hasLoad = false;
    }
}

///
/// @brief
///     定位到SCL元素。SCL元素为SCL文件的根元素
///
/// @return
///     true：定位成功；  false：找不到该元紿
///
bool SCLTools::FindSCL()
{
    if (m_hasLoad) {
        while(!m_reader->atEnd()) {
            if (m_reader->readNextStartElement()) {
                if (m_reader->name() == "SCL")
                    return true;
            }
        }
    }
    return false;
}

///
/// @brief
///     获取当前IED元素的属性，需先定位到IED元素
///
/// @param[in]
///     iedAttrib - 保存IED属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
bool SCLTools::GetIEDAttrib(SCL_IED_ATTRIB &iedAttrib)
{
    // 当前层级不是IED则返回false
    if (_tagName() != "IED")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    iedAttrib.iedName = attr.value("name").toString().toUtf8();
    iedAttrib.iedDesc = attr.value("desc").toString().toUtf8();
    iedAttrib.manufactactuer = attr.value("manufacturer").toString().toUtf8();
    iedAttrib.configVersion = attr.value("configVersion").toString().toUtf8();
    return true;
}

///
/// @brief
///     获取当前Server元素的属性，需先定位到Server元素
///
/// @param[in]
///     serverAttrib - 保存Server属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
bool SCLTools::GetServerAttrib(SCL_SERVER_ATTRIB &serverAttrib)
{
    // 当前层级不是AccessPoint则返回false
    if (_tagName() != "Services")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    serverAttrib.serverDesc = attr.value("desc").toString().toUtf8();
    serverAttrib.timeout    = attr.value("timeout").toString().toUtf8();
    return true;
}

///
/// @brief
///     获取当前AccessPoint元素的属性，需先定位到AccessPoint元素
///
/// @param[in]
///     apAttrib - 保存AccessPoint属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
bool SCLTools::GetAccessPointAttrib(SCL_ACCESS_POINT_ATTRIB &apAttrib)
{
    // 当前层级不是AccessPoint则返回false
    if (_tagName() != "AccessPoint")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    apAttrib.apDesc = attr.value("desc").toString().toUtf8();
    apAttrib.apName = attr.value("name").toString().toUtf8();

    if ( attr.value("clock") == "true" )
        apAttrib.clock = true;
    else
        apAttrib.clock = false;
    if ( attr.value("router")== "true" )
        apAttrib.router = true;
    else
        apAttrib.router = false;

    return true;
}

///
/// @brief
///     获取当前LDevice元素的属性，需先定位到LDevice元素
///
/// @param[in]
///     ldAttrib - 保存LDevice属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
bool SCLTools::GetLDeviceAttrib(SCL_LDEVICE_ATTRIB &ldAttrib)
{
    // 当前层级不是LDevice则返回false
    if (_tagName() != "LDevice")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    ldAttrib.ldInst = attr.value("inst").toString().toUtf8();
    ldAttrib.ldDesc = attr.value("desc").toString().toUtf8();
    return true;
}

///
/// @brief
///     获取当前LN0元素的属性，需先定位到LN0元素
///
/// @param[in]
///     ln0Attrib - 保存LN0属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
bool SCLTools::GetLN0Attrib(SCL_LN0_ATTRIB &ln0Attrib)
{
    // 当前层级不是LN0则返回false
    if (_tagName() != "LN0")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    ln0Attrib.lnInst  = attr.value("inst").toString().toUtf8();
    ln0Attrib.lnClass = attr.value("lnClass").toString().toUtf8();
    ln0Attrib.lnType  = attr.value("lnType").toString().toUtf8();
    ln0Attrib.lnDesc  = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取当前LN元素的属性，需先定位到LN元素
///
/// @param[in]
///     lnAttrib - 保存LN属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
bool SCLTools::GetLNAttrib(SCL_LN_ATTRIB &lnAttrib)
{
    // 当前层级不是LN则返回false
    if (_tagName() != "LN")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    lnAttrib.lnPrefix = attr.value("prefix").toString().toUtf8();
    lnAttrib.lnClass  = attr.value("lnClass").toString().toUtf8();
    lnAttrib.lnInst   = attr.value("inst").toString().toUtf8();
    lnAttrib.lnType   = attr.value("lnType").toString().toUtf8();
    lnAttrib.lnDesc   = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取当前DataSet元素的属性，需先定位到DataSet元素
///
/// @param[in]
///     dataSetAttrib - 保存DataSet属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
/// @Note:
///     仅获取DataSet的属性值，没有统计FCDA的个敿
///
bool SCLTools::GetDataSetAttrib(SCL_DATASET_ATTRIB &datSetAttrib)
{
    // 当前层级不是DataSet则返回false
    if (_tagName() != "DataSet")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    datSetAttrib.dataSetName = attr.value("name").toString().toUtf8();
    datSetAttrib.dataSetDesc  = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取DataSet中某一顺序号的FCDA元素的属性，需先定位到DataSet元素
///
/// @param[in]
///     fcdaAttrib - 保存FCDA属性信息的结构使
/// @param[in]
///     idxSeq     - 该FCDA元素在DataSet元素中的顺序号，仿开姿
///
/// @return
///     true：成功；  false：失贿
///
/// @Note:
///     仅获取FCDA的属性值，没有统计FCDA在DataSet中的位置
///
bool SCLTools::GetOneFCDAAttrib(SCL_FCDA_ATTRIB &fcdaAttrib)
{
    // 当前层级不是DataSet则返回false
    if (_tagName() != "FCDA")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    fcdaAttrib.ldInst = attr.value("ldInst").toString().toUtf8();
    fcdaAttrib.lnPrefix = attr.value("prefix").toString().toUtf8();
    fcdaAttrib.lnClass = attr.value("lnClass").toString().toUtf8();
    fcdaAttrib.lnInst = attr.value("lnInst").toString().toUtf8();
    fcdaAttrib.doName = attr.value("doName").toString().toUtf8();
    fcdaAttrib.daName = attr.value("daName").toString().toUtf8();
    fcdaAttrib.fc = attr.value("fc").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取DOI元素的属性，需先定位到DOI元素
///
/// @param[in]
///     doiAttrib - 保存DOI属性信息的结构使
///
/// @return
///     true：成功；  false：失贿
///
bool SCLTools::GetDOIAttrib(SCL_DOI_ATTRIB &doiAttrib)
{
    // 当前层级不是DOI则返回false
    if (_tagName() != "DOI")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    doiAttrib.doName = attr.value("name").toString().toUtf8();
    doiAttrib.doDesc = attr.value("desc").toString().toUtf8();
    // 	doiAttrib.accessControl = m_SCLFile.GetAttrib(_T("accessControl"));
    // 	doiAttrib.ix            = m_SCLFile.GetAttrib(_T("ix"));

    return true;
}

///
/// @brief
///     获取DAI元素的属性，需先定位到DAI元素
///
/// @param[in]
///     daiAttrib - 保存DAI属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDAIAttrib(SCL_DAI_ATTRIB &daiAttrib)
{
    // 当前层级不是DAI则返回false
    if (_tagName() != "DAI")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    daiAttrib.daName = attr.value("name").toString().toUtf8();
    daiAttrib.daDesc = attr.value("desc").toString().toUtf8();
    daiAttrib.sAddr = attr.value("sAddr").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取当前ExtRef元素的属性，需先定位到ExtRef元素
///
/// @param[in]
///     extRefAttrib - 保存ExtRef属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetExtRefAttrib(SCL_EXTREF_ATTRIB &extRefAttrib)
{
    // 当前层级不是ExtRef则返回false
    if (_tagName() != "ExtRef")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    extRefAttrib.desc       = attr.value("desc").toString().toUtf8();
    extRefAttrib.iedName    = attr.value("iedName").toString().toUtf8();
    extRefAttrib.ldInst     = attr.value("ldInst").toString().toUtf8();
    extRefAttrib.lnPrefix   = attr.value("prefix").toString().toUtf8();
    extRefAttrib.lnClass    = attr.value("lnClass").toString().toUtf8();
    extRefAttrib.lnInst     = attr.value("lnInst").toString().toUtf8();
    extRefAttrib.doName     = attr.value("doName").toString().toUtf8();
    extRefAttrib.daName     = attr.value("daName").toString().toUtf8();
    extRefAttrib.intAddr    = attr.value("intAddr").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取当前SmapledValueControl元素的属性，需先定位到SmapledValueControl元素
///
/// @param[in]
///     smvCtrlAttrib - 保存SmapledValueControl属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetSMVCtrlAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
    // 当前层级不是SampledValueControl则返回false
    if (_tagName() != "SampledValueControl")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    smvCtrlAttrib.cbName    = attr.value("name").toString().toUtf8();
    smvCtrlAttrib.cbDesc    = attr.value("desc").toString().toUtf8();
    smvCtrlAttrib.confRev   = attr.value("confRev").toString().toUtf8();
    smvCtrlAttrib.smvID     = attr.value("smvID").toString().toUtf8();
    smvCtrlAttrib.dataSet   = attr.value("datSet").toString().toUtf8();
    smvCtrlAttrib.smpRate   = attr.value("smpRate").toString().toUtf8();
    smvCtrlAttrib.nofASDU   = attr.value("nofASDU").toString().toUtf8();

    if (attr.value("multicast") == "false") 
        smvCtrlAttrib.multicast = false;
    else
        smvCtrlAttrib.multicast = true;

    // SmvOpts，SCL文件中默认为false
    if (m_reader->readNext() != QXmlStreamReader::StartElement) {
        smvCtrlAttrib.smvOptReftTm = false;
        smvCtrlAttrib.smvOptSmpRate = false;
        smvCtrlAttrib.smvOptSmpSync = false;
        smvCtrlAttrib.smvOptSecurity = false;
        smvCtrlAttrib.smvOptDataRef = false;
    }
    else {
        return GetSMVOptsAttrib(smvCtrlAttrib);
    }

    return true;
}

///
/// @brief
///     获取当前GSEControl元素的属性，需先定位到GSEControl元素
///
/// @param[in]
///     gseCtrlAttrib - 保存GSEControl属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetGSECtrlAttrib(SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
    // 当前层级不是GSEControl则返回false
    if (_tagName() != "GSEControl")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    gseCtrlAttrib.cbName    = attr.value("name").toString().toUtf8();
    gseCtrlAttrib.cbDesc    = attr.value("desc").toString().toUtf8();
    gseCtrlAttrib.confRev    = attr.value("confRev").toString().toUtf8();
    gseCtrlAttrib.gooseID    = attr.value("appID").toString().toUtf8();
    gseCtrlAttrib.dataSet    = attr.value("dataSet").toString().toUtf8();

    // type默认为GOOSE
    if (attr.value("type") == "GSSE") 
        gseCtrlAttrib.type = "GSSE";
    else
        gseCtrlAttrib.type = "GOOSE";

    return true;
}

///
/// @brief
///     获取SubNetwork元素的属性信息，需先定位到SubNetwork元素
///
/// @param[in]
///     subNetAttrib - 保存SubNetwork元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetSubNetworkAttrib(SCL_SUBNETWORK_ATTRIB &subNetAttrib)
{
    // 当前层级不是ConnectedAP则返回false
    if (_tagName() != "SubNetwork")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    subNetAttrib.name    = attr.value("name").toString().toUtf8();
    subNetAttrib.type    = attr.value("type").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取ConnectedAP元素的属性信息，该信息为在某子网上的访问点引用，需先定位到ConnectedAP元素
///
/// @param[in]
///     connectedAPAttrib - 保存ConnectedAP元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetConnectedAPAttrib(SCL_CONNECTEDAP_ATTRIB &connectedAPAttrib)
{
    // 当前层级不是ConnectedAP则返回false
    if (_tagName() != "ConnectedAP")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    connectedAPAttrib.iedName    = attr.value("iedName").toString().toUtf8();
    connectedAPAttrib.apName    = attr.value("apName").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取ConnectedAP元素中Address元素的信息，该信息为客户/服务器传输的地址信息，需先定位到Address元素
///
/// @param[in]
///     addrAtttrib - 保存ConnectedAP元素中Address元素的信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetAddrAttrib(SCL_ADDR_ATTRIB &addrAtttrib)
{
    // 当前层级不是Address则返回false
    if (_tagName() != "Address")
        return false;
    
    bool rc = false;
    if (hasChildNode()) {
        do {
            if (m_reader->name() == "P") {
                SCL_STR typeVal = m_reader->attributes().value("type").toString().toUtf8();
                SCL_STR pData   = m_reader->readElementText(QXmlStreamReader::SkipChildElements).toUtf8();
                if ( typeVal == "IP" )
                    addrAtttrib.ip = pData;
                else if ( typeVal == "IP-SUBNET" )
                    addrAtttrib.ipSubnet = pData;
                else if ( typeVal == "IP-GATEWAY" )
                    addrAtttrib.ipGateway = pData;
                else if ( typeVal == "OSI-NSAP" )
                    addrAtttrib.osiNsap = pData;
                else if ( typeVal == "OSI-TSEL" )
                    addrAtttrib.osiTsel = pData;
                else if ( typeVal == "OSI-SSEL" )
                    addrAtttrib.osiSsel = pData;
                else if ( typeVal == "OSI-PSEL" )
                    addrAtttrib.osiPsel = pData;
                else if ( typeVal == "OSI-AP-Title")
                    addrAtttrib.osiApTitle = pData;
                else if ( typeVal == "OSI-AP-Invoke")
                    addrAtttrib.osiApInvoke = pData;
                else if ( typeVal == "OSI-AE-Qualifier")
                    addrAtttrib.osiAeQualifier = pData;
                else if ( typeVal == "OSI-AE-Invoke")
                    addrAtttrib.osiAeInvoke = pData;
            }
            else {
                readNextEndElement();
            }
            if (m_reader->name() != "Address") {
                m_reader->readNextStartElement();
            }
            else {
                rc = true;
                break;
            }
        }while(m_reader->tokenType() != QXmlStreamReader::Invalid);
    }

    return rc;
}


///
/// @brief
///     获取ConnectedAP元素中SMV元素的信息，为SMV传输服务的通信地址及其控制块引用，需先定位到SMV元素
///
/// @param[in]
///     smvAttrib - 保存ConnectedAP元素中SMV信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetSMVAttrib(SCL_SMV_ATTRIB &smvAttrib)
{
    // 当前层级不是SMV则返回false
    if (_tagName() != "SMV")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    smvAttrib.ldInst    = attr.value("ldInst").toString().toUtf8();
    smvAttrib.cbName    = attr.value("cbName").toString().toUtf8();

    bool rc = false;
    if (hasChildNode()) {
        do {
            if (m_reader->name() == "Address") {
                rc = GetSMVorGSEAddressAttrib(smvAttrib.address);
                break;
            }
            else {
                readNextEndElement();
            }
            if (m_reader->name() != "SMV") {
                m_reader->readNextStartElement();
            }
            else {
                break;
            }
        }while(m_reader->tokenType() != QXmlStreamReader::Invalid);
    }
    else {
        return false;
    }

    return rc;
}

///
/// @brief
///     获取ConnectedAP元素中GSE元素的信息，为GOOSE/GSSE传输服务的通信地址及其控制块引用，需先定位到GSE元素
///
/// @param[in]
///     gseAttrib - 保存ConnectedAP元素中GSE信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetGSEAttrib(SCL_GSE_ATTRIB &gseAttrib)
{
    // 当前层级不是GSE则返回false
    if (_tagName() != "GSE")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    gseAttrib.ldInst    = attr.value("ldInst").toString().toUtf8();
    gseAttrib.cbName    = attr.value("cbName").toString().toUtf8();

    bool rc = false;
    // GSE-->Address，获取规定的地址信息
    if (hasChildNode()) {
        do {
            if (m_reader->name() == "Address") {
                rc = GetSMVorGSEAddressAttrib(gseAttrib.address);
                break;
            }
            else if (m_reader->name() == "MinTime") {
                QXmlStreamAttributes attr = m_reader->attributes();
                gseAttrib.minTimeUnit    = attr.value("multiplier").toString().toUtf8();
                gseAttrib.minTimeUnit    += attr.value("unit").toString().toUtf8();
                gseAttrib.minTimeVal     = m_reader->readElementText(QXmlStreamReader::SkipChildElements).toUtf8();
            }
            else if (m_reader->name() == "MaxTime") {
                QXmlStreamAttributes attr = m_reader->attributes();
                gseAttrib.maxTimeUnit    = attr.value("multiplier").toString().toUtf8();
                gseAttrib.maxTimeUnit    += attr.value("unit").toString().toUtf8();
                gseAttrib.maxTimeVal     = m_reader->readElementText(QXmlStreamReader::SkipChildElements).toUtf8();
            }
            else {
                readNextEndElement();
            }
            if (m_reader->name() != "GSE") {
                m_reader->readNextStartElement();
            }
            else {
                break;
            }
        }while(m_reader->tokenType() != QXmlStreamReader::Invalid);
    }
    else {
        return false;
    }

    return true;
}


///
/// @brief
///     获取LNodeType元素的属性信息，需先定位到LNodeType元素
///
/// @param[in]
///     lnTypeAttrib - 保存LNodeType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetLNTypeAttrib(SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
    // 当前层级不是LNodeType则返回false
    if (_tagName() != "LNodeType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    lnTypeAttrib.id         = attr.value("id").toString().toUtf8();
    lnTypeAttrib.desc       = attr.value("desc").toString().toUtf8();
    lnTypeAttrib.lnClass    = attr.value("lnClass").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取LNodeType中某一DO元素的属性信息，需先定位到DO元素
///
/// @param[in]
///     doAttrib - 保存DO元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDOAttrib(SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
    // 当前层级不是DO则返回false
    if (_tagName() != "DO")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    doAttrib.name         = attr.value("name").toString().toUtf8();
    doAttrib.type         = attr.value("type").toString().toUtf8();
    doAttrib.desc         = attr.value("desc").toString().toUtf8();

    return true;
}


///
/// @brief
///     获取DOType元素的属性信息，需先定位到DOType元素
///
/// @param[in]
///     doTypeAttrib - 保存DOType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDOTypeAttrib(SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
    // 当前层级不是DOType则返回false
    if (_tagName() != "DOType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    doTypeAttrib.id   = attr.value("id").toString().toUtf8();
    doTypeAttrib.cdc  = attr.value("cdc").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取DOType中某一DA元素的属性信息，需先定位到DA元素
///
/// @param[in]
///     daAttrib - 保存DA元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDAAttrib(SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
    // 当前层级不是DA则返回false
    if (_tagName() != "DA")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    daAttrib.name   = attr.value("name").toString().toUtf8();
    daAttrib.desc   = attr.value("desc").toString().toUtf8();
    daAttrib.fc     = attr.value("fc").toString().toUtf8();
    daAttrib.bType  = attr.value("bType").toString().toUtf8();
    daAttrib.type   = attr.value("type").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取DAType的属性信息，需先定位到DAType元素
///
/// @param[in]
///     daTypeAttrib - 保存DAType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetDATypeAttrib(SCL_DATYPE_ATTRIB &daTypeAttrib)
{
    // 当前层级不是DA则返回false
    if (_tagName() != "DAType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    daTypeAttrib.id   = attr.value("id").toString().toUtf8();
    daTypeAttrib.desc = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取DAType的属性信息，需先定位到DAType元素
///
/// @param[in]
///     daAttrib - 保存DAType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetBDAAttrib(SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
    // 当前层级不是DA则返回false
    if (_tagName() != "BDA")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    bdaAttrib.name   = attr.value("name").toString().toUtf8();
    bdaAttrib.bType  = attr.value("bType").toString().toUtf8();
    bdaAttrib.type   = attr.value("type").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取EnumType的属性信息，需先定位到EnumType元素
///
/// @param[in]
///     enumTypeAttrib - 保存EnumType元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
/// @note
///     没有统计EnumVal的个数
///
bool SCLTools::GetEnumTypeAttrib(SCL_ENUMTYPE_ATTRIB &enumTypeAttrib)
{
    // 当前层级不是EnumType则返回false
    if (_tagName() != "EnumType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    enumTypeAttrib.id    = attr.value("id").toString().toUtf8();
    enumTypeAttrib.desc  = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     获取EnumVal元素的属性信息，需先定位到EnumVal元素
///
/// @param[in]
///     enumValAttrib - 保存EnumVal元素的属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetEnumValAttrib(SCL_ENUMVAL_ATTRIB &enumValAttrib)
{
    // 当前层级不是EnumType则返回false
    if (_tagName() != "EnumType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    enumValAttrib.ord    = attr.value("ord").toString().toUtf8();
    enumValAttrib.enumVal  = m_reader->readElementText(QXmlStreamReader::SkipChildElements).toUtf8();

    return true;
}



















///
/// @brief
///     获取当前SmvOpts元素的属性，需先定位到SmvOpts元素
///
/// @param[in]
///     smvOptsAttrib - 保存SmvOpts属性信息的结构体
///
/// @return
///     true：成功；  false：失败
///
bool SCLTools::GetSMVOptsAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
    // 当前层级不是SmvOpts则返回false
    if (_tagName() != "SmvOpts")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();

    if (attr.value("refreshTime") == "true") 
        smvCtrlAttrib.smvOptReftTm = true;
    else
        smvCtrlAttrib.smvOptReftTm = false;

    if (attr.value("sampleRate") == "true") 
        smvCtrlAttrib.smvOptSmpRate = true;
    else
        smvCtrlAttrib.smvOptSmpRate = false;

    if (attr.value("sampleSynchronized") == "true") 
        smvCtrlAttrib.smvOptSmpSync = true;
    else
        smvCtrlAttrib.smvOptSmpSync = false;

    if (attr.value("security") == "true") 
        smvCtrlAttrib.smvOptSecurity = true;
    else
        smvCtrlAttrib.smvOptSecurity = false;

    if (attr.value("dataRef") == "true") 
        smvCtrlAttrib.smvOptDataRef = true;
    else
        smvCtrlAttrib.smvOptDataRef = false;

    return true;
}


bool SCLTools::GetSMVorGSEAddressAttrib(SCL_SMVorGSE_ADDRES_ATTRIB &addressAttrib)
{
    // 当前层级不是Address则返回false
    if (_tagName() != "Address")
        return false;

    bool rc = false;
    if (hasChildNode()) {
        do {
            if (m_reader->name() == "P") {
                SCL_STR typeVal = m_reader->attributes().value("type").toString().toUtf8();
                SCL_STR pData   = m_reader->readElementText(QXmlStreamReader::SkipChildElements).toUtf8();
                if ( typeVal == "MAC-Address" )
                    addressAttrib.macAddr = pData;
                else if ( typeVal == "APPID" )
                    addressAttrib.appID = pData;
                else if ( typeVal == "VLAN-ID" )
                    addressAttrib.vlanID = pData;
                else if ( typeVal == "VLAN-PRIORITY" )
                    addressAttrib.vlanPriority = pData;
            }
            else {
                readNextEndElement();
            }
            if (m_reader->name() != "Address") {
                m_reader->readNextStartElement();
            }
            else {
                rc = true;
                break;
            }
        }while(m_reader->tokenType() != QXmlStreamReader::Invalid);
    }

    return rc;
}