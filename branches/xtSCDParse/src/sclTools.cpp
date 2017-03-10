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
///     ��λ��SCLԪ�ء�SCLԪ��ΪSCL�ļ��ĸ�Ԫ��
///
/// @return
///     true����λ�ɹ���  false���Ҳ�����Ԫ�H
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
///     ��ȡ��ǰIEDԪ�ص����ԣ����ȶ�λ��IEDԪ��
///
/// @param[in]
///     iedAttrib - ����IED������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetIEDAttrib(SCL_IED_ATTRIB &iedAttrib)
{
    // ��ǰ�㼶����IED�򷵻�false
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
///     ��ȡ��ǰServerԪ�ص����ԣ����ȶ�λ��ServerԪ��
///
/// @param[in]
///     serverAttrib - ����Server������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetServerAttrib(SCL_SERVER_ATTRIB &serverAttrib)
{
    // ��ǰ�㼶����AccessPoint�򷵻�false
    if (_tagName() != "Services")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    serverAttrib.serverDesc = attr.value("desc").toString().toUtf8();
    serverAttrib.timeout    = attr.value("timeout").toString().toUtf8();
    return true;
}

///
/// @brief
///     ��ȡ��ǰAccessPointԪ�ص����ԣ����ȶ�λ��AccessPointԪ��
///
/// @param[in]
///     apAttrib - ����AccessPoint������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetAccessPointAttrib(SCL_ACCESS_POINT_ATTRIB &apAttrib)
{
    // ��ǰ�㼶����AccessPoint�򷵻�false
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
///     ��ȡ��ǰLDeviceԪ�ص����ԣ����ȶ�λ��LDeviceԪ��
///
/// @param[in]
///     ldAttrib - ����LDevice������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLDeviceAttrib(SCL_LDEVICE_ATTRIB &ldAttrib)
{
    // ��ǰ�㼶����LDevice�򷵻�false
    if (_tagName() != "LDevice")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    ldAttrib.ldInst = attr.value("inst").toString().toUtf8();
    ldAttrib.ldDesc = attr.value("desc").toString().toUtf8();
    return true;
}

///
/// @brief
///     ��ȡ��ǰLN0Ԫ�ص����ԣ����ȶ�λ��LN0Ԫ��
///
/// @param[in]
///     ln0Attrib - ����LN0������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLN0Attrib(SCL_LN0_ATTRIB &ln0Attrib)
{
    // ��ǰ�㼶����LN0�򷵻�false
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
///     ��ȡ��ǰLNԪ�ص����ԣ����ȶ�λ��LNԪ��
///
/// @param[in]
///     lnAttrib - ����LN������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLNAttrib(SCL_LN_ATTRIB &lnAttrib)
{
    // ��ǰ�㼶����LN�򷵻�false
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
///     ��ȡ��ǰDataSetԪ�ص����ԣ����ȶ�λ��DataSetԪ��
///
/// @param[in]
///     dataSetAttrib - ����DataSet������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
/// @Note:
///     ����ȡDataSet������ֵ��û��ͳ��FCDA�ĸ���
///
bool SCLTools::GetDataSetAttrib(SCL_DATASET_ATTRIB &datSetAttrib)
{
    // ��ǰ�㼶����DataSet�򷵻�false
    if (_tagName() != "DataSet")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    datSetAttrib.dataSetName = attr.value("name").toString().toUtf8();
    datSetAttrib.dataSetDesc  = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     ��ȡDataSet��ĳһ˳��ŵ�FCDAԪ�ص����ԣ����ȶ�λ��DataSetԪ��
///
/// @param[in]
///     fcdaAttrib - ����FCDA������Ϣ�Ľṹʹ
/// @param[in]
///     idxSeq     - ��FCDAԪ����DataSetԪ���е�˳��ţ��¿���
///
/// @return
///     true���ɹ���  false��ʧ��
///
/// @Note:
///     ����ȡFCDA������ֵ��û��ͳ��FCDA��DataSet�е�λ��
///
bool SCLTools::GetOneFCDAAttrib(SCL_FCDA_ATTRIB &fcdaAttrib)
{
    // ��ǰ�㼶����DataSet�򷵻�false
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
///     ��ȡDOIԪ�ص����ԣ����ȶ�λ��DOIԪ��
///
/// @param[in]
///     doiAttrib - ����DOI������Ϣ�Ľṹʹ
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDOIAttrib(SCL_DOI_ATTRIB &doiAttrib)
{
    // ��ǰ�㼶����DOI�򷵻�false
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
///     ��ȡDAIԪ�ص����ԣ����ȶ�λ��DAIԪ��
///
/// @param[in]
///     daiAttrib - ����DAI������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDAIAttrib(SCL_DAI_ATTRIB &daiAttrib)
{
    // ��ǰ�㼶����DAI�򷵻�false
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
///     ��ȡ��ǰExtRefԪ�ص����ԣ����ȶ�λ��ExtRefԪ��
///
/// @param[in]
///     extRefAttrib - ����ExtRef������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetExtRefAttrib(SCL_EXTREF_ATTRIB &extRefAttrib)
{
    // ��ǰ�㼶����ExtRef�򷵻�false
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
///     ��ȡ��ǰSmapledValueControlԪ�ص����ԣ����ȶ�λ��SmapledValueControlԪ��
///
/// @param[in]
///     smvCtrlAttrib - ����SmapledValueControl������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetSMVCtrlAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
    // ��ǰ�㼶����SampledValueControl�򷵻�false
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

    // SmvOpts��SCL�ļ���Ĭ��Ϊfalse
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
///     ��ȡ��ǰGSEControlԪ�ص����ԣ����ȶ�λ��GSEControlԪ��
///
/// @param[in]
///     gseCtrlAttrib - ����GSEControl������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetGSECtrlAttrib(SCL_GSECTRL_ATTRIB &gseCtrlAttrib)
{
    // ��ǰ�㼶����GSEControl�򷵻�false
    if (_tagName() != "GSEControl")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    gseCtrlAttrib.cbName    = attr.value("name").toString().toUtf8();
    gseCtrlAttrib.cbDesc    = attr.value("desc").toString().toUtf8();
    gseCtrlAttrib.confRev    = attr.value("confRev").toString().toUtf8();
    gseCtrlAttrib.gooseID    = attr.value("appID").toString().toUtf8();
    gseCtrlAttrib.dataSet    = attr.value("dataSet").toString().toUtf8();

    // typeĬ��ΪGOOSE
    if (attr.value("type") == "GSSE") 
        gseCtrlAttrib.type = "GSSE";
    else
        gseCtrlAttrib.type = "GOOSE";

    return true;
}

///
/// @brief
///     ��ȡSubNetworkԪ�ص�������Ϣ�����ȶ�λ��SubNetworkԪ��
///
/// @param[in]
///     subNetAttrib - ����SubNetworkԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetSubNetworkAttrib(SCL_SUBNETWORK_ATTRIB &subNetAttrib)
{
    // ��ǰ�㼶����ConnectedAP�򷵻�false
    if (_tagName() != "SubNetwork")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    subNetAttrib.name    = attr.value("name").toString().toUtf8();
    subNetAttrib.type    = attr.value("type").toString().toUtf8();

    return true;
}

///
/// @brief
///     ��ȡConnectedAPԪ�ص�������Ϣ������ϢΪ��ĳ�����ϵķ��ʵ����ã����ȶ�λ��ConnectedAPԪ��
///
/// @param[in]
///     connectedAPAttrib - ����ConnectedAPԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetConnectedAPAttrib(SCL_CONNECTEDAP_ATTRIB &connectedAPAttrib)
{
    // ��ǰ�㼶����ConnectedAP�򷵻�false
    if (_tagName() != "ConnectedAP")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    connectedAPAttrib.iedName    = attr.value("iedName").toString().toUtf8();
    connectedAPAttrib.apName    = attr.value("apName").toString().toUtf8();

    return true;
}

///
/// @brief
///     ��ȡConnectedAPԪ����AddressԪ�ص���Ϣ������ϢΪ�ͻ�/����������ĵ�ַ��Ϣ�����ȶ�λ��AddressԪ��
///
/// @param[in]
///     addrAtttrib - ����ConnectedAPԪ����AddressԪ�ص���Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetAddrAttrib(SCL_ADDR_ATTRIB &addrAtttrib)
{
    // ��ǰ�㼶����Address�򷵻�false
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
///     ��ȡConnectedAPԪ����SMVԪ�ص���Ϣ��ΪSMV��������ͨ�ŵ�ַ������ƿ����ã����ȶ�λ��SMVԪ��
///
/// @param[in]
///     smvAttrib - ����ConnectedAPԪ����SMV��Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetSMVAttrib(SCL_SMV_ATTRIB &smvAttrib)
{
    // ��ǰ�㼶����SMV�򷵻�false
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
///     ��ȡConnectedAPԪ����GSEԪ�ص���Ϣ��ΪGOOSE/GSSE��������ͨ�ŵ�ַ������ƿ����ã����ȶ�λ��GSEԪ��
///
/// @param[in]
///     gseAttrib - ����ConnectedAPԪ����GSE��Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetGSEAttrib(SCL_GSE_ATTRIB &gseAttrib)
{
    // ��ǰ�㼶����GSE�򷵻�false
    if (_tagName() != "GSE")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    gseAttrib.ldInst    = attr.value("ldInst").toString().toUtf8();
    gseAttrib.cbName    = attr.value("cbName").toString().toUtf8();

    bool rc = false;
    // GSE-->Address����ȡ�涨�ĵ�ַ��Ϣ
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
///     ��ȡLNodeTypeԪ�ص�������Ϣ�����ȶ�λ��LNodeTypeԪ��
///
/// @param[in]
///     lnTypeAttrib - ����LNodeTypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetLNTypeAttrib(SCL_LNTYPE_ATTRIB &lnTypeAttrib)
{
    // ��ǰ�㼶����LNodeType�򷵻�false
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
///     ��ȡLNodeType��ĳһDOԪ�ص�������Ϣ�����ȶ�λ��DOԪ��
///
/// @param[in]
///     doAttrib - ����DOԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDOAttrib(SCL_DOINLNTYPE_ATTRIB &doAttrib)
{
    // ��ǰ�㼶����DO�򷵻�false
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
///     ��ȡDOTypeԪ�ص�������Ϣ�����ȶ�λ��DOTypeԪ��
///
/// @param[in]
///     doTypeAttrib - ����DOTypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDOTypeAttrib(SCL_DOTYPE_ATTRIB &doTypeAttrib)
{
    // ��ǰ�㼶����DOType�򷵻�false
    if (_tagName() != "DOType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    doTypeAttrib.id   = attr.value("id").toString().toUtf8();
    doTypeAttrib.cdc  = attr.value("cdc").toString().toUtf8();

    return true;
}

///
/// @brief
///     ��ȡDOType��ĳһDAԪ�ص�������Ϣ�����ȶ�λ��DAԪ��
///
/// @param[in]
///     daAttrib - ����DAԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDAAttrib(SCL_DAINDOTYPE_ATTRIB &daAttrib)
{
    // ��ǰ�㼶����DA�򷵻�false
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
///     ��ȡDAType��������Ϣ�����ȶ�λ��DATypeԪ��
///
/// @param[in]
///     daTypeAttrib - ����DATypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetDATypeAttrib(SCL_DATYPE_ATTRIB &daTypeAttrib)
{
    // ��ǰ�㼶����DA�򷵻�false
    if (_tagName() != "DAType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    daTypeAttrib.id   = attr.value("id").toString().toUtf8();
    daTypeAttrib.desc = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     ��ȡDAType��������Ϣ�����ȶ�λ��DATypeԪ��
///
/// @param[in]
///     daAttrib - ����DATypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetBDAAttrib(SCL_BDAINDATYPE_ATTRIB &bdaAttrib)
{
    // ��ǰ�㼶����DA�򷵻�false
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
///     ��ȡEnumType��������Ϣ�����ȶ�λ��EnumTypeԪ��
///
/// @param[in]
///     enumTypeAttrib - ����EnumTypeԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
/// @note
///     û��ͳ��EnumVal�ĸ���
///
bool SCLTools::GetEnumTypeAttrib(SCL_ENUMTYPE_ATTRIB &enumTypeAttrib)
{
    // ��ǰ�㼶����EnumType�򷵻�false
    if (_tagName() != "EnumType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    enumTypeAttrib.id    = attr.value("id").toString().toUtf8();
    enumTypeAttrib.desc  = attr.value("desc").toString().toUtf8();

    return true;
}

///
/// @brief
///     ��ȡEnumValԪ�ص�������Ϣ�����ȶ�λ��EnumValԪ��
///
/// @param[in]
///     enumValAttrib - ����EnumValԪ�ص�������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetEnumValAttrib(SCL_ENUMVAL_ATTRIB &enumValAttrib)
{
    // ��ǰ�㼶����EnumType�򷵻�false
    if (_tagName() != "EnumType")
        return false;

    QXmlStreamAttributes attr = m_reader->attributes();
    enumValAttrib.ord    = attr.value("ord").toString().toUtf8();
    enumValAttrib.enumVal  = m_reader->readElementText(QXmlStreamReader::SkipChildElements).toUtf8();

    return true;
}



















///
/// @brief
///     ��ȡ��ǰSmvOptsԪ�ص����ԣ����ȶ�λ��SmvOptsԪ��
///
/// @param[in]
///     smvOptsAttrib - ����SmvOpts������Ϣ�Ľṹ��
///
/// @return
///     true���ɹ���  false��ʧ��
///
bool SCLTools::GetSMVOptsAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib)
{
    // ��ǰ�㼶����SmvOpts�򷵻�false
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
    // ��ǰ�㼶����Address�򷵻�false
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