#pragma once

#include <list>
#include <QStandardItemModel>

struct ImportData;

union eth_segment_info {
    struct {
        unsigned short pos;             //  ���ֶ��ڱ����е���ʼλ��
        unsigned short length;          //  ���ֶεı��ĳ���
    };
    unsigned int data;

    eth_segment_info(unsigned short pos = 0, unsigned short length = 0) {
        this->pos = pos;
        this->length = length;
    }

};
/*
 *  @Func:  ethFrameToItemModel
 *  @Desc:
 *      ����̫������ת����QStandardItemModel����ǰ������ȫ����SMV92��GOOSE��������ʽֻ������VLAN
 *  
 *  @Param[in]:
 *      frame�� ��̫������
 *      size:   ���ĵĳ���
 *  
 *  @Param[out]:
 *      model:  ����ת���������QStandardItemModel
 *  
 *  @Return:
 *      true��  ת���ɹ�
 */
bool ethFrameToItemModel(const unsigned char* frame, unsigned int size, QStandardItemModel* model);

/*
 *  @Func:  ethGetSegmentInfo
 *  @Desc��
 *      ��ȡValue�ֶε��û����ݣ�eth_segment_info��
 *  
 *  @Param[in]:
 *      item��  �������û�����(eth_segment_info)��QStandardItem��item������ethFrameToItemModelת�������ɵ�model
 *  
 *  @Param[out]:
 *      info��  ����item��Ӧ���û�����(eth_segment_info)
 *  
 *  @Return:
 *      true:   ��ȡ�û����ݳɹ�
 *      false:  ��ȡ�û�����ʧ�ܣ�item�Ƿ�
 */
bool ethGetSegmentInfo(QStandardItem* item, eth_segment_info &info);



/*
 *  @Func:  ethFindSegmentItem
 *  @Desc��
 *      ����pos����λ�õ�Item�����Item��row=0
 *  
 *  @Param[in]:
 *      model:          ��ethFrameToItemModel���ɵ�model
 *      segInfoItem:    ��¼��eth_segment_info���е�Item
 *      pos:            ��Ҫ���ұ��ĵ�λ��
 *  
 *  @Return:
 *      pos����λ�õ�Item�����Item��row=0
 *      NULL:           ����ʧ��
 */
QStandardItem* ethFindSegmentItem(QStandardItemModel *model, QStandardItem* segInfoItem, int pos);


bool ethFrameToImportData(const unsigned char* frame, unsigned int size, ImportData* importData);