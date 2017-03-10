#pragma once

#include <list>
#include <QStandardItemModel>

struct ImportData;

union eth_segment_info {
    struct {
        unsigned short pos;             //  该字段在报文中的起始位置
        unsigned short length;          //  该字段的报文长度
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
 *      将以太网报文转换成QStandardItemModel，当前可以完全解析SMV92、GOOSE，其他格式只解析到VLAN
 *  
 *  @Param[in]:
 *      frame： 以太网报文
 *      size:   报文的长度
 *  
 *  @Param[out]:
 *      model:  保存转换后的属性QStandardItemModel
 *  
 *  @Return:
 *      true：  转换成功
 */
bool ethFrameToItemModel(const unsigned char* frame, unsigned int size, QStandardItemModel* model);

/*
 *  @Func:  ethGetSegmentInfo
 *  @Desc：
 *      获取Value字段的用户数据（eth_segment_info）
 *  
 *  @Param[in]:
 *      item：  保存了用户数据(eth_segment_info)的QStandardItem，item必须是ethFrameToItemModel转换后生成的model
 *  
 *  @Param[out]:
 *      info：  返回item对应的用户数据(eth_segment_info)
 *  
 *  @Return:
 *      true:   获取用户数据成功
 *      false:  获取用户数据失败，item非法
 */
bool ethGetSegmentInfo(QStandardItem* item, eth_segment_info &info);



/*
 *  @Func:  ethFindSegmentItem
 *  @Desc：
 *      查找pos所在位置的Item，这个Item的row=0
 *  
 *  @Param[in]:
 *      model:          由ethFrameToItemModel生成的model
 *      segInfoItem:    记录了eth_segment_info那列的Item
 *      pos:            需要查找报文的位置
 *  
 *  @Return:
 *      pos所在位置的Item，这个Item的row=0
 *      NULL:           查找失败
 */
QStandardItem* ethFindSegmentItem(QStandardItemModel *model, QStandardItem* segInfoItem, int pos);


bool ethFrameToImportData(const unsigned char* frame, unsigned int size, ImportData* importData);