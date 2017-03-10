#include "pcapAnalysis.h"

#include <QDebug>
#include "MessageCodec/include/msg_rapid_decoder.h"

CPcapAnalysis::CPcapAnalysis()
{
    m_pcap = CreatePcapFileImpl();
    m_frame.clear();
    m_fileName = "";
}

CPcapAnalysis::~CPcapAnalysis()
{
    m_fileName = "";
    ReleasePcapFileImpl(m_pcap);
    clearFrame();
}

CPcapAnalysis* CPcapAnalysis::getInstance()
{
    static CPcapAnalysis anaylsis;

    return &anaylsis;
}

bool CPcapAnalysis::parseFile(const QString& fileName)
{
    if (fileName == "") {
        return false;
    }
    if (m_fileName == fileName) {
        return true;
    }
    m_pcap->Close();
    if (PCAP_FILE_SUCCESS != m_pcap->Open(fileName.toStdWString(), PFM_READ) ) {
        qDebug() << "load" << fileName << "failed.";
        return false;
    }
    m_fileName = fileName;

    clearFrame();

    if (PCAP_FILE_SUCCESS != m_pcap->GotoFirstPckToRead()) {
        return false;
    }
    MsgRapidDecoder msgDecoder;
    PCAP_PACKET_HEADER stPcapPckHeader;
    do {
        Frame *frame = new Frame;
        m_pcap->ReadOnePckHeader(stPcapPckHeader);
        if (stPcapPckHeader.capLen == stPcapPckHeader.len && 
            stPcapPckHeader.len != 0 &&
            stPcapPckHeader.len < 1514)
        {
            frame->len = stPcapPckHeader.len;
            frame->data = new quint8[frame->len];
            m_pcap->ReadOnePckData(frame->data, frame->len);
            msgDecoder.GetAppID(frame->data, frame->len, frame->appId);
            msgDecoder.GetDstMac(frame->data, frame->len, frame->mac);
            frame->type = msgDecoder.GetMessageType(frame->data, frame->len);
            m_frame.push_back(frame);
        }
        else {
            qDebug() << "capLen != len";
        }
    }while(m_pcap->GotoNextPckToRead() == PCAP_FILE_SUCCESS);

    return true;
}


int CPcapAnalysis::generateFrame(int item, QStandardItemModel &model)
{
    if (item < 0 || item >= m_frame.size()) {
        return -1;
    }
    Frame* frame = m_frame[item];

    return 0;
}


void CPcapAnalysis::clearFrame()
{
    for(FRAME_VECTOR::iterator it = m_frame.begin(); it != m_frame.end(); it++) {
        if ((*it)->data) {
            delete[] (*it)->data;
        }
        delete (*it);
    }
    m_frame.clear();
}