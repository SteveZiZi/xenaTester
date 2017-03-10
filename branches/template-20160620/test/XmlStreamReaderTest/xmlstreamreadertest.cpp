#include "xmlstreamreadertest.h"

#include <QtCore>
#include <QtGui>
#include <QtXml>
#include <qDebug>

#include <QXmlStreamWriter>

#define TEST_CASE
#include "../../src/xtSCDParse/src/sclTools.h"
typedef std::string SCL_STR;


void testReadDOI()
{
    SCLTools sclTools;

    sclTools.load(SCL_STR("test.xml"));

    qDebug() << "=====  testReadDOI ======";

    if (sclTools.FindElem(SCL_STR("SCL"))) {
        bool rc = false;
        while (sclTools.m_reader->readNext() != QXmlStreamReader::Invalid) {
            if (sclTools.tagName() == "testReadDOI") {
                rc = true;
                break;
            }
        }
        if (!rc) {
            qDebug() << "not found Test_DOI";
            goto exitCode;
        }

        sclTools.readNextStartElem();
        if (sclTools.tagName() != "DOI") {
            rc = false;
            qDebug() << "next StartElem is not DOI";
            goto exitCode;
        }

        if (rc) {
            SCL_DOI_ATTRIB doi;
            if (sclTools.GetDOIAttrib(doi)) {
                qDebug() << QString::fromUtf8(doi.doDesc.c_str()) 
                    << QString::fromUtf8(doi.doName.c_str()) 
                    << QString::fromUtf8(doi.dUSAddr.c_str()) 
                    << QString::fromUtf8(doi.dUVal.c_str());
            }
            else {
                qDebug() << "error";
            }
        }
        else {
            qDebug() << "Not Found DOI";
        }
    }
exitCode:
    sclTools.unload();
    qDebug() << "=====  testReadDOI ======";
}

void testGetAddrAttrib()
{
    SCLTools sclTools;

    sclTools.load(SCL_STR("test.xml"));

    qDebug() << "=====  testReadNextEndElem ======";

    if (sclTools.FindElem(SCL_STR("SCL"))) {
        bool rc = false;
        while (sclTools.m_reader->readNext() != QXmlStreamReader::Invalid) {
            if (sclTools.tagName() == "testGetAddrAttrib") {
                rc = true;
                break;
            }
        }
        if (!rc) {
            qDebug() << "not found testGetAddrAttrib";
            goto exitCode;
        }

        sclTools.readNextStartElem();
        if (sclTools.tagName() != "Address") {
            rc = false;
            qDebug() << "next StartElem is not Address";
            goto exitCode;
        }

        SCL_ADDR_ATTRIB addr;
        sclTools.GetAddrAttrib(addr);

    }
exitCode:
    sclTools.unload();
    qDebug() << "=====  testReadNextEndElem ======";
}


void testGetGSEAttrib()
{
    SCLTools sclTools;

    sclTools.load(SCL_STR("test.xml"));

    qDebug() << "=====  testGetSMVAttrib ======";

    if (sclTools.FindElem(SCL_STR("SCL"))) {
        bool rc = false;
        while (sclTools.m_reader->readNext() != QXmlStreamReader::Invalid) {
            if (sclTools.tagName() == "testGetGSEAttrib") {
                rc = true;
                break;
            }
        }
        if (!rc) {
            qDebug() << "not found testGetGSEAttrib";
            goto exitCode;
        }

        sclTools.readNextStartElem();
        if (sclTools.tagName() != "GSE") {
            rc = false;
            qDebug() << "next StartElem is not GSE";
            goto exitCode;
        }

        SCL_GSE_ATTRIB attr;
        sclTools.GetGSEAttrib(attr);

    }
exitCode:
    sclTools.unload();
    qDebug() << "=====  testGetGSEAttrib ======";
}


void testGetSubNetworkAttrib()
{
    SCLTools sclTools;

    sclTools.load(SCL_STR("test.xml"));

    qDebug() << "=====  testGetSubNetworkAttrib ======";

    if (sclTools.FindElem(SCL_STR("testGetSubNetworkAttrib"))) {
        while(sclTools.hasChildNode()) {
            if (sclTools.tagName() != "SubNetwork") {
                qDebug() << "next StartElem is not GSE";
                goto exitCode;
            }
            SCL_SUBNETWORK_ATTRIB subNetworkAttr;
            sclTools.GetSubNetworkAttrib(subNetworkAttr);
            qDebug() << subNetworkAttr.name.c_str();
            if (sclTools.hasChildNode()) {
                do 
                {
                    if (sclTools.tagName() == "ConnectedAP") {
                        SCL_CONNECTEDAP_ATTRIB connectApAttr;
                        sclTools.GetConnectedAPAttrib(connectApAttr);
                        qDebug() << connectApAttr.iedName.c_str() << connectApAttr.apName.c_str();
                        if (sclTools.hasChildNode()) {
                            do 
                            {
                                if (sclTools.tagName() == "SMV") {
                                    SCL_SMV_ATTRIB smvAttr;
                                    sclTools.GetSMVAttrib(smvAttr);
                                    qDebug() << smvAttr.address.appID.c_str();
                                }
                                else if (sclTools.tagName() == "GSE") {
                                    SCL_GSE_ATTRIB gseAttr;
                                    sclTools.GetGSEAttrib(gseAttr);
                                    qDebug() << gseAttr.address.appID.c_str();
                                }
                            } while (sclTools.hasSiblingNode());
                        }
                    }
                } while (sclTools.hasSiblingNode());
            }
        }
    }
    else {
        qDebug() << "not found testGetSubNetworkAttrib";
    }
exitCode:
    sclTools.unload();
    qDebug() << "=====  testGetSubNetworkAttrib ======";
}

void testWriter()
{
    QFile file("write.xml");
    file.open(QIODevice::WriteOnly |QIODevice::Truncate | QIODevice::Text);
    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);  
    writer.writeStartDocument();  

    writer.writeStartElement("Address");

    writer.writeStartElement("P");
    writer.writeAttribute("type", "dddd");
    writer.writeCharacters("0123456");
    writer.writeEndElement();

    writer.writeEndElement();

    writer.writeEndElement();  
    writer.writeEndDocument();  
    file.close();
}


XmlStreamReaderTest::XmlStreamReaderTest(QWidget *parent, Qt::WFlags flags)
    : QDialog(parent, flags)
{
    ui.setupUi(this);

//     testReadDOI();
//     testGetAddrAttrib();
//     testGetGSEAttrib();

    //testWriter();

    testGetSubNetworkAttrib();
}

XmlStreamReaderTest::~XmlStreamReaderTest()
{

}
