#pragma once

#include <vector>
#include "sclType.h"

#include "../inc/xtscdparse_global.h"

#include <QString>

class QFile;
class QXmlStreamReader;

class SCLTools
{
public:
    SCLTools();
    ~SCLTools();

    bool load(const SCL_STR &fileName);
    void unload();

    ///
    /// @brief
    ///     ��λ����
    ///     ��ĳһ�㼶�ж�λ֮ǰȷ���ѽ���ò㼶������IntoElem����
    ///     ��ͬһ�㼶�ڶ�λʱ���л�����ͬTag��Ԫ��ǰ����ResetMainPos������λ
    ///
    bool FindSCL();

    ///
    /// @brief
    ///     ȡֵ������ȡֵǰ���ȶ�λ����Ԫ�ش�
    ///
    bool GetIEDAttrib(SCL_IED_ATTRIB &iedAttrib);
    bool GetAccessPointAttrib(SCL_ACCESS_POINT_ATTRIB &apAttrib);
    bool GetServerAttrib(SCL_SERVER_ATTRIB &serverAttrib);
    bool GetLDeviceAttrib(SCL_LDEVICE_ATTRIB &ldAttrib);
    bool GetLN0Attrib(SCL_LN0_ATTRIB &ln0Attrib);
    bool GetLNAttrib(SCL_LN_ATTRIB &lnAttrib);
    bool GetDataSetAttrib(SCL_DATASET_ATTRIB &datSetAttrib);
    bool GetOneFCDAAttrib(SCL_FCDA_ATTRIB &fcdaAttrib);
    bool GetDOIAttrib(SCL_DOI_ATTRIB &doiAttrib);
    bool GetDAIAttrib(SCL_DAI_ATTRIB &daiAttrib);
    bool GetExtRefAttrib(SCL_EXTREF_ATTRIB &extRefAttrib);
    bool GetSMVCtrlAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
    bool GetGSECtrlAttrib(SCL_GSECTRL_ATTRIB &gseCtrlAttrib);

    bool GetSubNetworkAttrib(SCL_SUBNETWORK_ATTRIB &subNetAttrib);
    bool GetConnectedAPAttrib(SCL_CONNECTEDAP_ATTRIB &connectedAPAttrib);
    bool GetAddrAttrib(SCL_ADDR_ATTRIB &mmsAddr);
    bool GetSMVAttrib(SCL_SMV_ATTRIB &smvAttrib);
    bool GetGSEAttrib(SCL_GSE_ATTRIB &gseAttrib);

    bool GetLNTypeAttrib(SCL_LNTYPE_ATTRIB &lnTypeAttrib);
    bool GetDOAttrib(SCL_DOINLNTYPE_ATTRIB &doAttrib);
    bool GetDOTypeAttrib(SCL_DOTYPE_ATTRIB &doTypeAttrib);
    bool GetDAAttrib(SCL_DAINDOTYPE_ATTRIB &daAttrib);
    bool GetDATypeAttrib(SCL_DATYPE_ATTRIB &daTypeAttrib);
    bool GetBDAAttrib(SCL_BDAINDATYPE_ATTRIB &bdaAttrib);
    bool GetEnumTypeAttrib(SCL_ENUMTYPE_ATTRIB &enumTypeAttrib);
    bool GetEnumValAttrib(SCL_ENUMVAL_ATTRIB &enumValAttrib);

private:
    QString _tagName();
    bool readNextStartElem();
    bool hasChildNode();
    bool readNextEndElement();

    bool GetSMVOptsAttrib(SCL_SMVCTRL_ATTRIB &smvCtrlAttrib);
    bool GetSMVorGSEAddressAttrib(SCL_SMVorGSE_ADDRES_ATTRIB &addressAttrib);


private:
    QXmlStreamReader* m_reader;
    QFile* m_file;
    bool m_hasLoad;
};

#include <QXmlStreamReader>
inline QString SCLTools::_tagName()
{
    if (m_reader->tokenType() == QXmlStreamReader::StartElement) 
        return m_reader->name().toString();

    return QString();
}

inline bool SCLTools::readNextStartElem()
{
    return m_reader->readNextStartElement();
}

///
/// @brief
///     �жϵ�ǰ�ڵ��Ƿ����ӽڵ㣬�������ӽڵ㣬��ǰTokenType������StartElement
///
inline bool SCLTools::hasChildNode()
{
    if (m_reader->isStartElement()) {
        while(m_reader->readNext() != QXmlStreamReader::Invalid) {
            if (m_reader->isStartElement()) {
                return true;
            }
            else if (m_reader->isEndElement()) {
                return false;
            }
        }
    }

    return false;
}

inline bool SCLTools::readNextEndElement() 
{
    while(m_reader->readNext() != QXmlStreamReader::Invalid) {
        if (m_reader->isStartElement()) {
            return false;
        }
        else if (m_reader->isEndElement()) {
            return true;
        }
    }
    return false;
}
