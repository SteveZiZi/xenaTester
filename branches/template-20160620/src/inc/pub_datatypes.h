/// @file
///
/// @brief
///     定义程序使用的公用类型定义
///     包括主程序，各模块使用的通道类型，数值一次/二次类型，数据帧类型等
///
/// Copyright (c) 2016 广州炫通电气科技有限公司
///
/// 作者:
///    yangyuchao  2016.5.17
///
/// 修改历史：
///

#pragma once


///
/// @brief
///    定义数据帧(报文)类型
///
enum enumFrameType{
    FRAME_TYPE_UNKOWN = 0,                                              ///< 数据帧类型未知
    FRAME_TYPE_NET_GOOSE,                                               ///< 网络goose数据帧
    FRAME_TYPE_NET_9_1,                                                 ///< 网络9-1数据帧，12个通道
	FRAME_TYPE_NET_9_1_EX,                                              ///< 网络9-1扩展数据帧（由南京新宁光电定义，最多12个通道）
    FRAME_TYPE_NET_9_2,                                                 ///< 网络9-2数据帧
    FRAME_TYPE_NET_1588,                                                ///< 网络1588报文
    FRAME_TYPE_FT3_NANRUI,                                              ///< 南瑞使用标准帧（56字节版本），12个通道
    FRAME_TYPE_FT3_GUOWANG,                                             ///< 国网使用扩展帧，22个通道
	FRAME_TYPE_FT3_STD52,                                               ///< FT3标准帧（52字节版本），12个通道
	FRAME_TYPE_FT3_ONEPHASE_CTORVT,                                     ///< 单相电流/电压互感器FT3数据帧，5个通道
	FRAME_TYPE_FT3_TRIPHASE_CT,                                         ///< 三相电流互感器FT3数据帧，9个通道
	FRAME_TYPE_FT3_TRIPHASE_VT_2010,                                    ///< 三相电压互感器FT3数据帧（2010版），6个通道
	FRAME_TYPE_FT3_TRIPHASE_VT_2011,                                    ///< 三相电压互感器FT3数据帧（2011年以后的版本），6个通道
	FRAME_TYPE_FT3_TRIPHASE_CTORVT                                      ///< 三相电流/电压互感器FT3数据帧，15个通道
};

///
/// @brief
///    描述通道类型信息，包括smv、goose通道类型
///
enum enumChannelType{
    CHANNEL_TYPE_UNKOWN  = 0,                                           ///< 通道类型未知
    //smv通道类型
    CHANNEL_TYPE_SMV_CURRENT,                                           ///< 通道类型为SMV电流
    CHANNEL_TYPE_SMV_VOLTAGE,                                           ///< 通道类型为SMV电压
    CHANNEL_TYPE_SMV_TIME,                                              ///< 通道类型为SMV时间(指9-2中的延时)
    //goose通道类型
    CHANNEL_TYPE_GOOSE_STRUCT,                                          ///< 结构
    CHANNEL_TYPE_GOOSE_POINT,                                           ///< 单点
    CHANNEL_TYPE_GOOSE_DOUBLE_POINT,                                    ///< 双点
    CHANNEL_TYPE_GOOSE_QUALITY,                                         ///< 品质
    CHANNEL_TYPE_GOOSE_TIME,                                            ///< 通道类型为goose时间信息
    CHANNEL_TYPE_GOOSE_FLOAT,                                           ///< 单精度
    CHANNEL_TYPE_GOOSE_INT8,                                            ///< 有符号8位整数
    CHANNEL_TYPE_GOOSE_INT16,                                           ///< 有符号16位整数
    CHANNEL_TYPE_GOOSE_INT24,                                           ///< 有符号24位整数
    CHANNEL_TYPE_GOOSE_INT32,                                           ///< 有符号32位整数
    CHANNEL_TYPE_GOOSE_UINT8,                                           ///< 无符号8位整数
    CHANNEL_TYPE_GOOSE_UINT16,                                          ///< 无符号16位整数
    CHANNEL_TYPE_GOOSE_UINT24,                                          ///< 无符号24位整数
    CHANNEL_TYPE_GOOSE_UINT32                                           ///< 无符号32位整数
};

///
/// @brief
///    描述通道是保护类型数据还是测量数据
///
enum enumChannelValueType
{
    CHANNEL_VALUE_TYPE_UNKNOWN = 0,                                     ///< 通道数据类型为未知
    CHANNEL_VALUE_TYPE_PROTECT,                                         ///< 通道数据类型为保护值
    CHANNEL_VALUE_TYPE_MEASURE                                          ///< 通道数据类型为测量值
};

///
/// @brief
///    采样值数据类型是一次/二次值,
///
enum enumSmvValueType
{
    SMV_VALUE_TYPE_ORIGINANL = 0,                                       ///< 报文数据为原始值
    SMV_VALUE_TYPE_PRIMARY,                                             ///< 报文数据为一次值
    SMV_VALUE_TYPE_SECONDARY                                            ///< 报文数据为二次值
};


///
/// @brief
///    描述通道相别信息
///
enum ChannelPhase{
    CHANNEL_PHASE_UNKOWN  = 0,                                          ///< 通道相别未知
    CHANNEL_PHASE_A = 1,                                                ///< 通道相别为A相
    CHANNEL_PHASE_B = 2,                                                ///< 通道相别为B相
    CHANNEL_PHASE_C = 3,                                                ///< 通道相别为C相
    CHANNEL_PHASE_X = 4,                                                ///< 通道相别为X相，同期/母线/抽取
    CHANNEL_PHASE_N = 5                                                 ///< 通道相别为N相，中性点
};