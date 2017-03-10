#include "port.h"
#include "module.h"
#include "stream.h"

#include <QDebug>

void CPort::dumpMsg()
{
    qDebug( "++++++++++++  port:%d  ++++++++++", itemOfModule());

    qDebug() << "portName:" << m_portName;
    qDebug() << "realSpeed:" << m_nRealSpeed << "Mbps";
    qDebug() << "linked:" << m_bLinked <<"actived:" << m_bActived;
    qDebug() << "reserved:" << m_bReserved << "reservedBy:" << m_reservedBy;
    qDebug() << "maxHeaderLength:" << m_nMaxHeaderLength << "maxStreamNum" << m_nMaxStreamNum;
    qDebug() << "txTimeLimit:" << m_nTxTimeLimit;

    Q_FOREACH(CStream* stream_, m_streams)
        stream_->dumpMsg();

    qDebug( "------------  port:%d  ----------", itemOfModule());
}


CPort::CPort(CModule* module_/* = NULL*/)
: m_module(module_)
, m_portName("empty Port")
, m_nRealSpeed(1000)
, m_nMaxHeaderLength(128)
, m_nMaxStreamNum(32)
, m_nTxTimeLimit(0)
, m_bLinked(false)
, m_bActived(false)
, m_bReserved(false)
, m_isBadPort(true)
, m_fRateFraction(0.0)
, m_iPacketRate(0)
, m_dBitRate(0.0)
{

}

CPort::~CPort()
{
    Q_FOREACH(CStream* stream_, m_streams)
        if (stream_) delete stream_;
}

int CPort::itemOfModule() const
{
    if (m_module) 
        return m_module->findPort(this);

    return -1;
}


int CPort::unUsedIndexOfStream()
{
    int index = 0;

    do {
        bool unused = true;
        for(int i = 0; i < streamCount(); i++) {
            if (index == stream(i)->itemOfPort()) {
                unused = false;
                break;
            }
        }
        if (unused) {
            return index;
        }
        index++;
    } while(1);

    return -1;
}

CStream* CPort::stream(quint16 item) const
{
    if (item < m_streams.size()) {
        return m_streams.at(item);
    }
    return NULL;
}

int CPort::findStream(const CStream* stream_) const
{
    if (!stream_) {
        return -1;
    }

    int item = 0;
    Q_FOREACH(CStream* s, m_streams) {
        if (s == stream_) {
            return item;
        }
        item++;
    }

    return -1;
}
// 
// bool streamIndexLessThan(const CStream * s1, const CStream *s2)
// {
//     return s1->itemOfPort() < s2->itemOfPort();
// }

bool CPort::appendStream(CStream* stream_)
{
    if (!stream_) {
        return false;
    }

    m_streams.push_back(stream_);
    stream_->setPort(this);

    //qSort(m_streams.begin(), m_streams.end(), streamIndexLessThan);

    return true;
}

CStream* CPort::removeStream(CStream *stream_)
{
    return removeStream(findStream(stream_));
}

CStream* CPort::removeStream(int streamIndex)
{
    if (streamIndex < 0 || streamIndex >= streamCount())
    {
        return NULL;
    }

    return m_streams.takeAt(streamIndex);
}

void CPort::setMaxHeaderLength(int maxLen)
{
    if (maxLen <= 128) {
        maxLen = 128;
    }
    else if (maxLen <= 256) {
        maxLen = 256;
    }
    else if (maxLen <= 512) {
        maxLen = 512;
    }
    else {
        maxLen = 1024;
    }

    m_nMaxHeaderLength = maxLen; 
    m_nMaxStreamNum = 4096/maxLen;
}


void CPort::setPortName(const QString &name)
{
    if (m_module) {
        m_portName = QString("Port%1 %2").arg(itemOfModule()).arg(name);
    }
    else {
        m_portName = name;
    }
}


bool CPort::toXml(const CPort *port, QDomDocument &doc, QDomElement &portDom)
{
    if (!port) {
        return false;
    }

    QDomElement child = doc.createElement("isBadPort");
    child.setAttribute(QString("val"), port->m_isBadPort);
    portDom.appendChild(child);

    child = doc.createElement("MaxHeaderLength");
    child.setAttribute(QString("val"), port->m_nMaxHeaderLength);
    portDom.appendChild(child);

    child = doc.createElement("MaxStreamNum");
    child.setAttribute(QString("val"), port->m_nMaxStreamNum);
    portDom.appendChild(child);

    child = doc.createElement("TxTimeLimit");
    child.setAttribute(QString("val"), port->m_nTxTimeLimit);
    portDom.appendChild(child);

    child = doc.createElement("RealSpeed");
    child.setAttribute(QString("val"), port->m_nRealSpeed);
    portDom.appendChild(child);

    child = doc.createElement("Reserved");
    child.setAttribute(QString("val"), port->m_bReserved);
    child.setAttribute(QString("name"), port->m_reservedBy);
    portDom.appendChild(child);

    child = doc.createElement("TxMode");
    child.setAttribute(QString("val"), port->m_sTxMode);
    portDom.appendChild(child);

    child = doc.createElement("RateFraction");
    child.setAttribute(QString("val"), port->m_fRateFraction);
    portDom.appendChild(child);

    child = doc.createElement("PacketRate");
    child.setAttribute(QString("val"), port->m_iPacketRate);
    portDom.appendChild(child);

    child = doc.createElement("BitRate");
    child.setAttribute(QString("val"), port->m_dBitRate);
    portDom.appendChild(child);

    child = doc.createElement("streams");
    Q_FOREACH(CStream* stream_, port->m_streams)
    {
        QDomElement streamDom = doc.createElement("stream");
        streamDom.setAttribute(QString("item"), stream_->itemOfPort());
        if (!CStream::toXml(stream_, doc, streamDom)) {
            return false;
        }
        child.appendChild(streamDom);
    }
    portDom.appendChild(child);

    return true;
}

bool CPort::fromXml(QDomElement &portDom, CPort &port)
{
    try
    {
        QDomElement ele = portDom.firstChildElement(QString("isBadPort"));
        port.m_isBadPort = ele.attribute(QString("val"), QString("0")).toInt() ? true : false;

        ele = portDom.firstChildElement(QString("MaxHeaderLength"));
        port.m_nMaxHeaderLength = ele.attribute(QString("val")).toInt();

        ele = portDom.firstChildElement(QString("MaxStreamNum"));
        port.m_nMaxStreamNum = ele.attribute(QString("val")).toInt();

        ele = portDom.firstChildElement(QString("TxTimeLimit"));
        port.m_nTxTimeLimit = ele.attribute(QString("val")).toInt();

        ele = portDom.firstChildElement(QString("RealSpeed"));
        port.m_nRealSpeed = ele.attribute(QString("val")).toInt();

        ele = portDom.firstChildElement(QString("Reserved"));
        port.m_bReserved = ele.attribute(QString("val")).toInt() ? true : false;
        port.m_reservedBy = ele.attribute(QString("name"));

        ele = portDom.firstChildElement(QString("TxMode"));
        port.m_sTxMode = ele.attribute(QString("val"));

        ele = portDom.firstChildElement(QString("RateFraction"));
        port.m_fRateFraction = ele.attribute(QString("val")).toDouble();

        ele = portDom.firstChildElement(QString("PacketRate"));
        port.m_iPacketRate = ele.attribute(QString("val")).toInt();

        ele = portDom.firstChildElement(QString("BitRate"));
        port.m_dBitRate = ele.attribute(QString("val")).toDouble();

        ele = portDom.firstChildElement(QString("streams"));
        {
            QDomElement streamEle = ele.firstChildElement(QString("stream"));
            port.m_streams.clear();
            while(!streamEle.isNull()) {
                CStream* stream = new CStream();
                if (CStream::fromXml(streamEle, *stream)) {
                    port.appendStream(stream);
                }
                else {
                    return false;
                }
                streamEle = streamEle.nextSiblingElement();
            }
        }
    }
    catch(...)
    {

        return false;
    }

    return true;
}