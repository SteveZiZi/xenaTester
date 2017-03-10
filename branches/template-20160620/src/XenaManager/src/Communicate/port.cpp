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
, m_portName("exception")
, m_nRealSpeed(1000)
, m_nMaxHeaderLength(128)
, m_nMaxStreamNum(32)
, m_nTxTimeLimit(0)
, m_bLinked(false)
, m_bActived(false)
, m_bReserved(false)
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

bool CPort::appendStream(CStream* stream_)
{
    if (!stream_) {
        return false;
    }

    m_streams.push_back(stream_);
    stream_->setPort(this);
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