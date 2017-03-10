#pragma once

#include <QString>
#include <QtGui/QStandardItemModel>
#include <vector>
#include "inc\pub_datatypes.h"

#include "PcapFile/include/pcap_file_i.h"

class CPcapAnalysis {
public:
    enum {MAC_ADDR_LENTH = 6};

    struct Frame {
        quint32 len;
        enumFrameType type;
        quint16 appId;
        quint8 mac[MAC_ADDR_LENTH];
        quint8* data;
        Frame() : len(0), data(NULL), type(FRAME_TYPE_UNKOWN) {}
    };
    typedef std::vector<Frame*> FRAME_VECTOR;

    static CPcapAnalysis* getInstance();

    bool parseFile(const QString& fileName);

    int generateFrame(int item, QStandardItemModel &model);
protected:
    CPcapAnalysis();
    ~CPcapAnalysis();

private:
    void clearFrame();

private:
    IPcapFile* m_pcap;
    FRAME_VECTOR m_frame;
    QString m_fileName;

    friend class CPcapShowDialog;
};