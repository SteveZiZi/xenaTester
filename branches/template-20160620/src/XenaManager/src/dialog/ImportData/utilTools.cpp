#include "utilTools.h"

#include <Windows.h>
#include <QtGlobal>

#include "inc/pub_datatypes.h"

/*
 *  以太网报文的VLAN固定标记
 */
#define NET_VLAN_TYPE    (0x8100)


/*
 *  以太网目的MAC地址在报文中的位置
 */
#define NET_DEST_MAC_POS     (0)

/*
 *  以太网源MAC地址在报文中的位置
 */
#define NET_SOURCE_MAC_POS   (6)

/*
 *  以太网报文的VLAN标记在报文中的位置
 */
#define NET_VLAN_POS         (12)

/*
 *  带VLAN的以太网报文的报文类型在报文中的位置
 */
#define NET_FRAME_TYPE_POS   (16)

/*
 *  不带VLAN的以太网报文的报文类型在报文中的位置
 */
#define NET_noVLAN_FRAME_TYPE_POS    (12)

/*
 *  带VLAN的以太网报文的APPID在报文中的位置
 */
#define NET_APPID_POS   (18)

/*
 *  不带VLAN的以太网报文的APPID在报文中的位置
 */
#define NET_noVLAN_APPID_POS (14)


/*
 *  带VLAN的以太网报文的PDU在报文中的位置
 */
#define NET_PDU_POS   (26)

/*
 *  不带VLAN的以太网报文的PDU在报文中的位置
 */
#define NET_noVLAN_PDU_POS (22)


#define MAKE_WORD_BIGEND(Byte0, Byte1) ((Byte0)<<8|(Byte1))
#define MAKE_DWORD_BIGEND(Byte0, Byte1, Byte2, Byte3) ((Byte0)<<24|(Byte1<<16)|(Byte2)<<8|(Byte3))


#define MAX_ASDU_NUM  8


#define DECODE_ERROR  ((UINT16)-1)

typedef enum NET_FRAME_VLAN_e{
    FRAME_INC_VLAN = 0,
    FRAME_NO_VLAN,
    FRAME_VLAN_UNKNOW
};

enum _SMV92_FIELD_e{
    Smv92_Field_savPdu = 0x60,
    Smv92_Field_noASDU = 0x80,
    Smv92_Field_security = 0x81,
    Smv92_Field_sequenceOfAsdu = 0xA2,
};
enum _SMV92_ASDU_FIELD_e {
    Smv92_Field_Asdu_no = 0x30,
    Smv92_Field_Asdu_svId = 0x80,
    Smv92_Field_Asdu_dataSet,
    Smv92_Field_Asdu_smpCnt,
    Smv92_Field_Asdu_confRev,
    Smv92_Field_Asdu_referTm,
    Smv92_Field_Asdu_smpSynch,
    Smv92_Field_Asdu_smpRate,
    Smv92_Field_Asdu_sequenceOfData,
};

enum _GOOSE_FIELD_e {
    Goose_Field_Pdu = 0x61,
    Goose_Field_gocbReg = 0x80,
    Goose_Field_TTL,
    Goose_Field_dataSet,
    Goose_Field_goID,
    Goose_Field_eventTimestamp,
    Goose_Field_StNum,
    Goose_Field_SqNum,
    Goose_Field_TestMode,
    Goose_Field_confRev,
    Goose_Field_ndsCom,
    Goose_Field_numDataSetEntries,
    Goose_Field_allData = 0xAB,
    Goose_Field_security = 0x8C
};

enum _GOOSE_CHNL_TYPE {
    Goose_Chnl_array = 0xA1,
    Goose_Chnl_structure,
    Goose_Chnl_boolean = 0x83,
    Goose_Chnl_bitString,
    Goose_Chnl_integer,
    Goose_Chnl_unsigned,
    Goose_Chnl_float,
    Goose_Chnl_octetString,
    Goose_Chnl_visibleString,
    Goose_Chnl_generalTime,
    Goose_Chnl_binaryTime,
    Goose_Chnl_BCD,
    Goose_Chnl_booleanArray = 0xAD,
    Goose_Chnl_objId = 0x8E,
    Goose_Chnl_mmsString,
    Goose_Chnl_utcTime
};



static BOOL HadVlanType(const quint8* msg, UINT16 len) {
    Q_ASSERT(msg);
    if (len < NET_VLAN_POS+2) {
        return FALSE;
    }
    return (NET_VLAN_TYPE == MAKE_WORD_BIGEND(msg[NET_VLAN_POS], msg[NET_VLAN_POS+1]));
}

static UINT16 GetTLVLength(const quint8* pTLV, UINT16 len, UINT16 *pusLength = NULL) {
    Q_ASSERT(pTLV);
    Q_ASSERT(len >= 2);
    if (pTLV[1] < 0x80) {
        if (pusLength) {
            *pusLength = pTLV[1];
        }
        return 2;
    }
    else if (pTLV[1] == 0x81) {
        if (pusLength) {
            *pusLength = pTLV[2];
        }
        return 3;
    }
    else if (pTLV[1] == 0x82) {
        if (pusLength) {
            *pusLength = MAKE_WORD_BIGEND(pTLV[2], pTLV[3]);
        }
        return 4;
    }
    else {
        return DECODE_ERROR;
    }
}

#define USER_DATA_ITEM  1

static int addEthernetSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos);
static int addAppidSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos, enumFrameType frameType);
static int addUnknownSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos);
static int addGoosePduSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos);
static int addSmv92PduSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos);
static int addSmvDataSegment(const quint8* frame, quint32 size, QStandardItem* asduSeg, int pos, int dataLength);
static int addAsduSegment(const quint8* frame, quint32 size, QStandardItemModel* model, QStandardItem* pduSeg, int pos, int asduNum);

bool ethFrameToItemModel(const quint8* frame, quint32 size, QStandardItemModel* model)
{
   {
       QStringList headerName;
       headerName << "Segment" << "Value";
       model->setHorizontalHeaderLabels(headerName);
   }

   UINT16 pos = 0;
   QString text;
   ///
   ///  Ethernet
   ///
   pos = addEthernetSegment(frame, size, model, 0);

   enumFrameType frameType = FRAME_TYPE_UNKOWN;
   int frameTypePos = NET_FRAME_TYPE_POS;
   if (!HadVlanType(frame, size)) {
        frameTypePos = NET_noVLAN_FRAME_TYPE_POS;
   }
   switch (MAKE_WORD_BIGEND(frame[frameTypePos], frame[frameTypePos+1])) {
   case 0x88B8:
       frameType = FRAME_TYPE_NET_GOOSE;
       break;
   case 0x88BA:
       frameType = FRAME_TYPE_NET_9_2;
       break;
   default:
       frameType = FRAME_TYPE_UNKOWN;
       break;
   }

   if (frameType != FRAME_TYPE_UNKOWN) {
       /// 
       ///  appId
       /// 
        pos = addAppidSegment(frame, size, model, pos, frameType);

        if (frameType == FRAME_TYPE_NET_GOOSE) {
            pos = addGoosePduSegment(frame, size, model, pos);
        }
        else if (frameType == FRAME_TYPE_NET_9_2) {
            pos = addSmv92PduSegment(frame, size, model, pos);
        }
   }

    if (pos < size) {
        pos = addUnknownSegment(frame, size, model, pos);
    }

    return true;
}


bool ethGetSegmentInfo(QStandardItem* item, eth_segment_info &info)
{
    if ( item &&
        item->column() == USER_DATA_ITEM)
    {
        QVariant var = item->data();
        if (var.type() == QVariant::UInt) {
            info.data = var.toUInt();
            return true;
        }
    }

    return false;
}

QStandardItem* ethFindSegmentItem(QStandardItemModel *model, QStandardItem* segInfoItem, int pos)
{
    eth_segment_info info;
    ethGetSegmentInfo(segInfoItem, info);
    if (info.pos+info.length <= pos) {
        return NULL;
    }
    else {
        QModelIndex index = segInfoItem->index();
        index = model->index(index.row(), 0, index.parent());
        QStandardItem *siblingItem = model->itemFromIndex(index);
        if (siblingItem->rowCount()) {
            for(int row = 0; row < siblingItem->rowCount(); row++) {
                QStandardItem* childItem = siblingItem->child(row, 1);
                childItem = ethFindSegmentItem(model, childItem, pos);
                if (childItem) {
                    return childItem;
                }
                continue;
            }
        }
        else {
            return siblingItem;
        }
    }

    return NULL;
}


#define AddSeg(seg, subItem, desc, text, pos, len)                          \
    seg->setChild(subItem, 0, new QStandardItem(desc));                     \
    seg->setChild(subItem, 1, new QStandardItem(text.toUpper()));           \
    seg->child(subItem,USER_DATA_ITEM)->setData(QVariant(eth_segment_info(pos,len).data)); \
    seg->child(subItem,0)->setEditable(false);                              \
    pos += len;                                                             \
    subItem++

static int addEthernetSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos)
{
   int pos = curPos;
   QString text;
   ///
   ///  Ethernet
   ///
   int subItem = 0;
   QStandardItem *ethSeg = new QStandardItem("Ethernet");
   QStandardItem *infoSeg = new QStandardItem();
   ethSeg->setEditable(false);
   QList<QStandardItem*> itemList;
   itemList << ethSeg << infoSeg;
   model->appendRow(itemList);
   infoSeg->setEditable(false);

   // Dest Mac
   text = QString("%1 %2 %3 %4 %5 %6")
                   .arg(frame[pos+0], 2, 16, QChar('0'))
                   .arg(frame[pos+1], 2, 16, QChar('0'))
                   .arg(frame[pos+2], 2, 16, QChar('0'))
                   .arg(frame[pos+3], 2, 16, QChar('0'))
                   .arg(frame[pos+4], 2, 16, QChar('0'))
                   .arg(frame[pos+5], 2, 16, QChar('0'));
   AddSeg(ethSeg, subItem, "Dest MAC:", text, pos, 6);

   // Src Mac
   text = QString("%1 %2 %3 %4 %5 %6")
                   .arg(frame[pos+0], 2, 16, QChar('0'))
                   .arg(frame[pos+1], 2, 16, QChar('0'))
                   .arg(frame[pos+2], 2, 16, QChar('0'))
                   .arg(frame[pos+3], 2, 16, QChar('0'))
                   .arg(frame[pos+4], 2, 16, QChar('0'))
                   .arg(frame[pos+5], 2, 16, QChar('0'));
   AddSeg(ethSeg, subItem, "Src MAC:", text, pos, 6);

   // VLAN 
   if (HadVlanType(frame, size)) {
        // VLAN TAG
       text = QString("%1 %2")
                       .arg(frame[pos+0], 2, 16, QChar('0'))
                       .arg(frame[pos+1], 2, 16, QChar('0'));
       AddSeg(ethSeg, subItem, "VLAN TAG:", text, pos, 2);

        // VLAN TCI
       text = QString("%1 %2")
                       .arg(frame[pos+0], 2, 16, QChar('0'))
                       .arg(frame[pos+1], 2, 16, QChar('0'));
       AddSeg(ethSeg, subItem, "VLAN TCI:", text, pos, 2);
   }

   // Ethernet Type
   text = QString("%1 %2")
                   .arg(frame[pos+0], 2, 16, QChar('0'))
                   .arg(frame[pos+1], 2, 16, QChar('0'));
   AddSeg(ethSeg, subItem, "Ethernet Type:", text, pos, 2);

   infoSeg->setData(QVariant(eth_segment_info(curPos,pos-curPos).data));

   return pos;
}

static int addAppidSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos, enumFrameType frameType)
{
    int pos = curPos;
    QString text;
    ///
    ///  IEC-GOOSE
    ///
    int subItem = 0;
    QStandardItem *seg = new QStandardItem();
    seg->setEditable(false);
    if (frameType == FRAME_TYPE_NET_GOOSE) {
        seg->setText("IEC-GOOSE");
    }
    else if (frameType == FRAME_TYPE_NET_9_2) {
        seg->setText("SV 9-2");
    }
    else if (frameType == FRAME_TYPE_NET_9_1 || FRAME_TYPE_NET_9_1_EX) {
        seg->setText("SV 9-1");
    }
    else {
        return pos;
    }

    QList<QStandardItem*> itemList;
    QStandardItem *infoSeg = new QStandardItem();
    itemList << seg << infoSeg;
    model->appendRow(itemList);
    infoSeg->setEditable(false);
    // APPID
    text = QString("%1 %2")
                    .arg(frame[pos+0], 2, 16, QChar('0'))
                    .arg(frame[pos+1], 2, 16, QChar('0'));
    AddSeg(seg, subItem, "APPID:", text, pos, 2);

    //APP leght
    text = QString("%1 %2")
                    .arg(frame[pos+0], 2, 16, QChar('0'))
                    .arg(frame[pos+1], 2, 16, QChar('0'));
    AddSeg(seg, subItem, "App Legth:", text, pos, 2);

    //Reserved1
    text = QString("%1 %2")
            .arg(frame[pos+0], 2, 16, QChar('0'))
            .arg(frame[pos+1], 2, 16, QChar('0'));
    AddSeg(seg, subItem, "Reserved1:", text, pos, 2);

    //Reserved2
    text = QString("%1 %2")
        .arg(frame[pos+0], 2, 16, QChar('0'))
        .arg(frame[pos+1], 2, 16, QChar('0'));
    AddSeg(seg, subItem, "Reserved2:", text, pos, 2);

    infoSeg->setData(QVariant(eth_segment_info(curPos,pos-curPos).data));

    return pos;
}

#define UNKNOWN_SEG_DATA_LENGTH 16
static int addUnknownSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos)
{
    ///
    ///  Unknown
    ///
    int pos = curPos;
    int subItem = 0;
    QString text;
    QStandardItem *seg = new QStandardItem("Unknown");
    seg->setEditable(false);
    QList<QStandardItem*> itemList;
    QStandardItem *infoSeg = new QStandardItem();
    itemList << seg << infoSeg;
    model->appendRow(itemList);
    infoSeg->setEditable(false);
    int subItemNum = (size-pos)/UNKNOWN_SEG_DATA_LENGTH;
    for(int i = 0; i < subItemNum; i++) {
        text.clear();
        for(int j = pos; j < pos+UNKNOWN_SEG_DATA_LENGTH; j++) {
            text.append(QString("%1 ").arg(frame[j], 2, 16, QChar('0')));
        }
        AddSeg(seg, subItem, QString("Raw%1").arg(i+1), text, pos, UNKNOWN_SEG_DATA_LENGTH);
    }
    if (pos < size) {
        text.clear();
        for(int j = pos; j < size; j++) {
            text.append(QString("%1 ").arg(frame[j], 2, 16, QChar('0')));
        }
        AddSeg(seg, subItem, QString("Raw%1").arg(subItemNum+1), text, pos, size-pos);
    }

    infoSeg->setData(QVariant(eth_segment_info(curPos,pos-curPos).data));

    return size;
}

static int addGoosePduSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos)
{
    int pos = curPos;
    if (frame[pos] != 0x61) {
        return addUnknownSegment(frame, size, model, pos);
    }

    QString text;
    ///
    ///  PDU
    ///
    int subItem = 0;
    QStandardItem *seg = new QStandardItem("PDU");
    seg->setEditable(false);
    QList<QStandardItem*> itemList;
    QStandardItem *infoSeg = new QStandardItem();
    itemList << seg << infoSeg;
    model->appendRow(itemList);
    infoSeg->setEditable(false);
    // PDU Length
    UINT16 tlv_l = 0;
    int tlv_vPos = GetTLVLength(frame+pos, size-pos, NULL);
    for(int i = pos; i < pos+tlv_vPos; i++) {
        text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
    }
    AddSeg(seg, subItem, "PDU Length:", text, pos, tlv_vPos);
    

    // public seg
    QString desc;
    bool bSequenceOfData = false;
    bool bOk = false;
    do 
    {
        switch(frame[pos]&0x8F) {
        case 0x80:
            desc = "gocbRef:";
            break;
        case 0x81:
            desc = "TTL:";
            break;
        case 0x82:
            desc = "DataSet:";
            break;
        case 0x83:
            desc = "GoID:";
            break;
        case 0x84:
            desc = "Event Timestamp:";
            break;
        case 0x85:
            desc = "stNum:";
            break;
        case 0x86:
            desc = "sqNum:";
            break;
        case 0x87:
            desc = "Test Mode:";
            break;
        case 0x88:
            desc = "Config Rev:";
            break;
        case 0x89:
            desc = "Needs Commissioning:";
            break;
        case 0x8A:
            desc = "Entries Number:";
            break;
        case 0x8B:
            desc = "Sequence of Data:";
            bSequenceOfData = true;
            bOk = true;
            break;
        case 0x8C:
            desc = "security:";
            break;
        default:
            bOk = true;
            break;
        }

        if (bSequenceOfData) {
            tlv_vPos = GetTLVLength(frame+pos, size-pos, NULL);
            text.clear();
            for(int i = pos; i < pos+tlv_vPos; i++) {
                text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
            }
            AddSeg(seg, subItem, desc, text, pos, tlv_vPos);
        }
        else {
            text.clear();
            tlv_vPos = GetTLVLength(frame+pos, size-pos, &tlv_l);
            for(int i = pos; i < pos+tlv_vPos+tlv_l; i++) {
                text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
            }
            AddSeg(seg, subItem, desc, text, pos, tlv_vPos+tlv_l);
        }
    } while (!bOk && pos < size);


    if (!bSequenceOfData) {
        pos = addUnknownSegment(frame, size, model, pos);
    }
    else {
        QStandardItem * dataSeg = new QStandardItem("Datas");
        dataSeg->setEditable(false);
        QList<QStandardItem*> itemList;
        QStandardItem *infoSeg1 = new QStandardItem();
        itemList << dataSeg << infoSeg1;
        model->appendRow(itemList);
        infoSeg->setEditable(false);
        subItem = 0;
        int subPos = pos;
        //data
        int chnl = 0;
        while (pos < size) {
            text.clear();
            tlv_vPos = GetTLVLength(frame+pos, size-pos, &tlv_l);
            for(int i = pos; i < pos+tlv_vPos+tlv_l; i++) {
                text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
            }
            desc = QString("[%1]").arg(++chnl, 3, 10, QChar('0'));
            AddSeg(dataSeg, subItem, desc, text, pos, tlv_vPos+tlv_l);
        }
        infoSeg1->setData(QVariant(eth_segment_info(subPos,pos-subPos).data));
    }

    infoSeg->setData(QVariant(eth_segment_info(curPos,pos-curPos).data));

    return pos;
}

static int addSmv92PduSegment(const quint8* frame, quint32 size, QStandardItemModel* model, int curPos)
{
    int pos = curPos;
    if (frame[pos] != 0x60) {
        return addUnknownSegment(frame, size, model, pos);
    }

    QString text;
    ///
    ///  PDU
    ///
    int subItem = 0;
    QStandardItem *seg = new QStandardItem("PDU");
    seg->setEditable(false);
    QList<QStandardItem*> itemList;
    QStandardItem *infoSeg = new QStandardItem();
    itemList << seg << infoSeg;
    model->appendRow(itemList);
    infoSeg->setEditable(false);
    // PDU Length
    UINT16 tlv_l = 0;
    int tlv_vPos = GetTLVLength(frame+pos, size-pos, NULL);
    for(int i = pos; i < pos+tlv_vPos; i++) {
        text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
    }
    AddSeg(seg, subItem, "PDU Length:", text, pos, tlv_vPos);

    //  ASDU Number
    bool Ok = false;
    QString desc;
    bool bSequenceOfAsdu = false;
    bool getAsduNum = false;
    int asduNum = 0;
    do 
    {
        switch(frame[pos]) {
        case Smv92_Field_noASDU:
            desc = "ASDU Num:";
            getAsduNum = true;
            break;
        case Smv92_Field_security:
            desc = "security:";
            break;
        case Smv92_Field_sequenceOfAsdu:
            desc = "Sequence of ASDU:";
            bSequenceOfAsdu = true;
            Ok = true;
            break;
        default:
            Ok = true;
            break;
        }

        if (bSequenceOfAsdu) {
            tlv_vPos = GetTLVLength(frame+pos, size-pos, NULL);
            text.clear();
            for(int i = pos; i < pos+tlv_vPos; i++) {
                text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
            }
            AddSeg(seg, subItem, desc, text, pos, tlv_vPos);

            pos = addAsduSegment(frame, size, model, seg, pos, asduNum);
        }
        else {
            text.clear();
            tlv_vPos = GetTLVLength(frame+pos, size-pos, &tlv_l);
            for(int i = pos; i < pos+tlv_vPos+tlv_l; i++) {
                text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
            }
            AddSeg(seg, subItem, desc, text, pos, tlv_vPos+tlv_l);
            if (getAsduNum) {
                asduNum = frame[pos-1];
            }
        }
    } while (!Ok && pos < size);

    /// 
    ///  ASDU
    /// 
    if (!bSequenceOfAsdu) {
        pos = addUnknownSegment(frame, size, model, pos);
    }

    infoSeg->setData(QVariant(eth_segment_info(curPos,pos-curPos).data));

    return pos;
}

int addSmvDataSegment(const quint8* frame, quint32 size, QStandardItem* asduSeg, int pos, int dataLength)
{
    int subItem = 0;
    int subPos = pos;
    int chnlNum = dataLength/8;
    QStandardItem * dataSeg = new QStandardItem("Datas");
    dataSeg->setEditable(false);
    QList<QStandardItem*> itemList;
    QStandardItem *infoSeg = new QStandardItem();
    itemList << dataSeg << infoSeg;
    asduSeg->appendRow(itemList);
    infoSeg->setEditable(false);

    //chnl
    QString text;
    QString desc;
    for(int chnl = 0; chnl < chnlNum; chnl++) {
        text.clear();
        for(int i = pos; i < pos+8; i++) {
            text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
        }
        desc = QString("[%1]").arg(chnl+1, 2, 10, QChar('0'));
        AddSeg(dataSeg, subItem, desc, text, pos, 8);
    }
    infoSeg->setData(QVariant(eth_segment_info(subPos,pos-subPos).data));
    return pos;
}

int addAsduSegment(const quint8* frame, quint32 size, QStandardItemModel* model, QStandardItem* pduSeg, int pos, int asduNum)
{
    bool Ok = false;
    bool bSequenceOfData = false;
    QString text;
    QString desc;

    for(UINT16 i = 0; i < asduNum; i++) {
        if (frame[pos] != Smv92_Field_Asdu_no) {
            pos = addUnknownSegment(frame, size, model, pos);
            break;
        }

        int asduPos = pos;
        int subItem = 0;
        QStandardItem *asduSeg = new QStandardItem(QString("ASDU[%1]").arg(i));
        asduSeg->setEditable(false);
        QList<QStandardItem*> itemList;
        QStandardItem *infoSeg = new QStandardItem();
        itemList << asduSeg << infoSeg;
        pduSeg->appendRow(itemList);
        infoSeg->setEditable(false);

        // ASDU Length
        UINT16 tlv_l;
        int tlv_vPos = GetTLVLength(frame+pos, size-pos, NULL);
        text.clear();
        for(int i = pos; i < pos+tlv_vPos; i++) {
            text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
        }
        AddSeg(asduSeg, subItem, "ASDU Length:", text, pos, tlv_vPos);

        do 
        {
            switch(frame[pos]) {
            case Smv92_Field_Asdu_svId:
                desc = "SvID:";
                break;
            case Smv92_Field_Asdu_dataSet:
                desc = "DataSet:";
                break;
            case Smv92_Field_Asdu_smpCnt:
                desc = "Sample Count:";
                break;
            case Smv92_Field_Asdu_confRev:
                desc = "Config Rev:";
                break;
            case Smv92_Field_Asdu_referTm:
                desc = "Refer Time:";
                break;
            case Smv92_Field_Asdu_smpSynch:
                desc = "Sample Sync:";
                break;
            case Smv92_Field_Asdu_smpRate:
                desc = "Sample Rate:";
                break;
            case Smv92_Field_Asdu_sequenceOfData:
                desc = "Sequence of Data:";
                bSequenceOfData = true;
                Ok = true;
                break;
            default:
                Ok = true;
                break;
            }

            if (bSequenceOfData) {
                tlv_vPos = GetTLVLength(frame+pos, size-pos, &tlv_l);
                text.clear();
                for(int i = pos; i < pos+tlv_vPos; i++) {
                    text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
                }
                AddSeg(asduSeg, subItem, desc, text, pos, tlv_vPos);

                pos = addSmvDataSegment(frame, size, asduSeg, pos, tlv_l);
            }
            else {
                text.clear();
                tlv_vPos = GetTLVLength(frame+pos, size-pos, &tlv_l);
                for(int i = pos; i < pos+tlv_vPos+tlv_l; i++) {
                    text.append(QString("%1 ").arg(frame[i], 2, 16, QChar('0')));
                }
                AddSeg(asduSeg, subItem, desc, text, pos, tlv_vPos+tlv_l);
            }
        } while (!Ok && pos < size);

        if (!bSequenceOfData) {
            pos = addUnknownSegment(frame, size, model, pos);
        }
        infoSeg->setData(QVariant(eth_segment_info(asduPos,pos-asduPos).data));
    }

    return pos;
}