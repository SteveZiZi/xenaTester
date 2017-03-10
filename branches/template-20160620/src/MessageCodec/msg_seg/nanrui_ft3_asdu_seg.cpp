/// @file
///     nanrui_ft3_asdu_seg.cpp
///
/// @brief
///     南瑞FT3采样值报文的ASDU段
///
/// @note
///
/// Copyright (c) 2013 广州炫通电气科技有限公司
///
/// 作者：
///    yangyuchao  2013.3.18
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "nanrui_ft3_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     南瑞FT3采样值报文的ASDU的格式
///
typedef enum {
	NANRUI_FT3_ASDU_LEN = 0,
	NANRUI_FT3_LN_NAME,
	NANRUI_FT3_DATASET_NAME,
	NANRUI_FT3_LD_NAME,
	NANRUI_FT3_PHS_ARTG,
	NANRUI_FT3_NEUT_ARTG,
	NANRUI_FT3_PHS_VRTG,
	NANRUI_FT3_DELAY_TIME,
	NANRUI_FT3_RESERVED,
	NANRUI_FT3_CRC1,
	NANRUI_FT3_DATA_CHANNEL_1,
	NANRUI_FT3_DATA_CHANNEL_2,
	NANRUI_FT3_DATA_CHANNEL_3,
	NANRUI_FT3_DATA_CHANNEL_4,
	NANRUI_FT3_DATA_CHANNEL_5,
	NANRUI_FT3_DATA_CHANNEL_6,
	NANRUI_FT3_DATA_CHANNEL_7,
	NANRUI_FT3_DATA_CHANNEL_8,
	NANRUI_FT3_CRC2,
	NANRUI_FT3_DATA_CHANNEL_9,
	NANRUI_FT3_DATA_CHANNEL_10,
	NANRUI_FT3_DATA_CHANNEL_11,
	NANRUI_FT3_DATA_CHANNEL_12,
	NANRUI_FT3_STATUS_WORD_1,
	NANRUI_FT3_STATUS_WORD_2,
	NANRUI_FT3_SMPCNT,
	NANRUI_FT3_SMPRATE,
	NANRUI_FT3_CONFREV,
	NANRUI_FT3_CRC3
}NANRUI_FT3_ASDU_FORMAT;

///
/// @brief
///     对应上面的ASDU格式，南瑞FT3采样值报文的ASDU各字段占用的字节
///
static const uint8_t g_NanRui_FT3_Asdu_Size_List[] = {
	2,1,1,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,1,1,2
};

NanRuiFT3AsduSeg::NanRuiFT3AsduSeg()
: m_AsduData(new std::vector<MsgDataElem *>)
, m_AsduLen(0)
, m_AsduOff(0)
{
	m_AsduData->reserve(sizeof(g_NanRui_FT3_Asdu_Size_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_NanRui_FT3_Asdu_Size_List)/sizeof(uint8_t); i++ )
	{
		m_AsduData->push_back(CreateMsgDataElemImpl(DT_BASE_DATA));
	}
}

NanRuiFT3AsduSeg::~NanRuiFT3AsduSeg()
{
	if ( m_AsduData != NULL )
	{
		std::vector<MsgDataElem *>::iterator it;
		for ( it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
		{
			ReleaseMsgDataElemImpl(*it);
		}
		delete m_AsduData;
		m_AsduData = NULL;
	}
}

///
/// @brief
///     对报文帧中的ASDU部分进行解码
///
/// @param[in]
///     offset      - 待解码的ASDU在报文帧中的偏移地址
/// @param[in]
///     asduDataBuf - 保存待解码的ASDU数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t NanRuiFT3AsduSeg::Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize)
{
	if ( bufSize < NANRUI_FT3_MSG_WORD-1 )
		return 0;

	size_t dSize  = 0;
	size_t offTmp = 0;
	uint8_t *pBuf = (uint8_t *)asduDataBuf;

	// 根据定义好的格式依次解析各字段数据
	int i = NANRUI_FT3_ASDU_LEN;
	for ( ; i <= NANRUI_FT3_CRC3; i++ )
	{
		if ( (offTmp/sizeof(uint16_t)) >= bufSize )
			break;

		if ( i == NANRUI_FT3_CRC1 || i == NANRUI_FT3_CRC2 || i == NANRUI_FT3_CRC3 )
		{
			// 跳过CRC校验
			offTmp += g_NanRui_FT3_Asdu_Size_List[i];
		}
		else
		{
			dSize = m_AsduData->at(i)->Decode(offset+offTmp,&pBuf[offTmp],g_NanRui_FT3_Asdu_Size_List[i]);
			if ( dSize == 0 )
				return 0;
			offTmp += dSize;
		}
	}

	m_AsduLen = offTmp;
	m_AsduOff = offset;
	return (offTmp/sizeof(uint16_t));
}

///
/// @brief
///     对设置的参数进行编码，获得ASDU数据流，不包含CRC校验
///
/// @param[in]
///     asduDataArr - 保存ASDU数据流的数组
/// @param[in]
///     arrSize     - 数组大小
///
/// @return
///     保存ASDU数据流的数组的实际大小 - 成功； 0 - 失败
///
size_t NanRuiFT3AsduSeg::Code(uint16_t *asduDataArr, const size_t arrSize)
{
	if ( arrSize < NANRUI_FT3_MSG_WORD-1 )
		return 0;

	uint16_t bufTmp[NANRUI_FT3_MSG_WORD-1];
	size_t   cSize  = 0;
	size_t   offTmp = 0;
	uint8_t *pBuf   = (uint8_t *)bufTmp;
	
	// 根据定义好的格式依次解析各字段数据
	int i = NANRUI_FT3_ASDU_LEN;
	for ( ; i <= NANRUI_FT3_CRC3; i++ )
	{
		if ( (offTmp/sizeof(uint16_t)) >= arrSize )
			break;

		if ( i == NANRUI_FT3_CRC1 || i == NANRUI_FT3_CRC2 || i == NANRUI_FT3_CRC3 )
		{
			// 跳过CRC校验
			continue;
		}
		else
		{
			cSize = m_AsduData->at(i)->Code(&pBuf[offTmp],arrSize*sizeof(uint16_t)-offTmp);
			offTmp += cSize;
		}
	}

	// 对编码数据进行大小端转换
	for ( size_t i = 0; i < offTmp/sizeof(uint16_t); i++ )
		asduDataArr[i] = BSWAP_16(bufTmp[i]);

	m_AsduLen = offTmp;
	return (offTmp/sizeof(uint16_t));
}

///
/// @brief
///     获取ASDU的公共信息，即ASDU中的采样值数据以外的部分
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool NanRuiFT3AsduSeg::GetASDUPublicInfo(const uint16_t *msgDataBuf, NANRUI_FT3_ASDU_PUBLIC &asduPublic)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_ASDU_LEN)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.asduLen = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_LN_NAME)->GetValueOffset()];
	asduPublic.lnName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_DATASET_NAME)->GetValueOffset()];
	asduPublic.dataSetName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_LD_NAME)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.ldName = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_PHS_ARTG)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.phsArtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_NEUT_ARTG)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.neutArtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_PHS_VRTG)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.phsVrtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_DELAY_TIME)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.delayTime = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_STATUS_WORD_1)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord1 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_STATUS_WORD_2)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord2 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_SMPCNT)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.smpCnt = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_SMPRATE)->GetValueOffset()];
	asduPublic.smpRate = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(NANRUI_FT3_CONFREV)->GetValueOffset()];
	asduPublic.confRev = *uint8Buf;
	return true;
}

///
/// @brief
///     获取报文帧中的ASDU中的采样值数据部分的通道数
///
/// @return
///     ASDU中的采样值数据部分的通道数
///
size_t NanRuiFT3AsduSeg::GetASDUSmpsNum()
{
	return NANRUI_FT3_ASDU_CHANNEL_NUM;
}

///
/// @brief
///     获取ASDU中的采样值数据
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     smpValArr  - 保存采样瞬时值的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     数组的实际大小
///
size_t NanRuiFT3AsduSeg::GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;
	int i = NANRUI_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= NANRUI_FT3_DATA_CHANNEL_12; i++ )
	{
		if ( smpNum == arrSize )
			break;
		if ( i == NANRUI_FT3_CRC2 )
			continue;
		uint8Buf = &pMsgBuf[m_AsduData->at(i)->GetValueOffset()];
		memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
		smpValArr[smpNum] = BSWAP_16(uint16Tmp);
		smpNum++;
	}
	return smpNum;
}

///
/// @brief
///     获取ASDU的长度
///
/// @return
///     ASDU的长度
///
size_t NanRuiFT3AsduSeg::GetASDULen()
{
	return m_AsduLen;
}

///
/// @brief
///     获取ASDU在报文帧中的偏移地址
///
/// @return
///     ASDU在报文帧中的偏移地址
///
size_t NanRuiFT3AsduSeg::GetASDUOff()
{
	return m_AsduOff;
}

///
/// @brief
///     获取ASDU的公共信息在报文帧中的偏移地址信息
///
/// @param[in]
///     asduPublicOff - 保存ASDU的公共信息的偏移地址信息的结构体
///
/// @return
///     
///
void NanRuiFT3AsduSeg::GetASDUPublicInfoOff(NANRUI_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	asduPublicOff.asduLenOff     = m_AsduData->at(NANRUI_FT3_ASDU_LEN)->GetElemOffset();
	asduPublicOff.lnNameOff      = m_AsduData->at(NANRUI_FT3_LN_NAME)->GetElemOffset();
	asduPublicOff.dataSetNameOff = m_AsduData->at(NANRUI_FT3_DATASET_NAME)->GetElemOffset();
	asduPublicOff.ldNameOff      = m_AsduData->at(NANRUI_FT3_LD_NAME)->GetElemOffset();
	asduPublicOff.phsArtgOff     = m_AsduData->at(NANRUI_FT3_PHS_ARTG)->GetElemOffset();
	asduPublicOff.neutArtgOff    = m_AsduData->at(NANRUI_FT3_NEUT_ARTG)->GetElemOffset();
	asduPublicOff.phsVrtgOff     = m_AsduData->at(NANRUI_FT3_PHS_VRTG)->GetElemOffset();
	asduPublicOff.delayTimeOff   = m_AsduData->at(NANRUI_FT3_DELAY_TIME)->GetElemOffset();
	asduPublicOff.reservedOff    = m_AsduData->at(NANRUI_FT3_RESERVED)->GetElemOffset();
	asduPublicOff.statusWord1Off = m_AsduData->at(NANRUI_FT3_STATUS_WORD_1)->GetElemOffset();
	asduPublicOff.statusWord2Off = m_AsduData->at(NANRUI_FT3_STATUS_WORD_2)->GetElemOffset();
	asduPublicOff.smpCntOff      = m_AsduData->at(NANRUI_FT3_SMPCNT)->GetElemOffset();
	asduPublicOff.smpRateOff     = m_AsduData->at(NANRUI_FT3_SMPRATE)->GetElemOffset();
	asduPublicOff.confRevOff     = m_AsduData->at(NANRUI_FT3_CONFREV)->GetElemOffset();
}

///
/// @brief
///     获取ASDU中的采样值数据在报文帧中的偏移地址信息
///
/// @param[in]
///     smpOffArr - 保存采样瞬时值的偏移地址信息的数组
/// @param[in]
///     arrSize   - 数组大小
///
/// @return
///     数组的实际大小
///
size_t NanRuiFT3AsduSeg::GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize)
{
	int i = NANRUI_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= NANRUI_FT3_DATA_CHANNEL_12; i++ )
	{
		if ( smpNum == arrSize )
			break;
		if ( i == NANRUI_FT3_CRC2 )
			continue;
		smpOffArr[smpNum] = m_AsduData->at(i)->GetElemOffset();
		smpNum++;
	}
	return smpNum;
}

///
/// @brief
///     设置ASDU信息
///
/// @param[in]
///     asduPublic - ASDU的公共信息
/// @param[in]
///     smpValBuf  - 保存采样瞬时值的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     
///
void NanRuiFT3AsduSeg::SetASDU(const NANRUI_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize)
{
	uint16_t uint16Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	// 设置公共信息
	uint16Tmp = 0x2C;
	uint16Tmp = BSWAP_16(uint16Tmp);
	m_AsduData->at(NANRUI_FT3_ASDU_LEN)->SetValue((uint8_t *)&uint16Tmp,2);
	uint8Tmp  = asduPublic.lnName;
	m_AsduData->at(NANRUI_FT3_LN_NAME)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.dataSetName;
	m_AsduData->at(NANRUI_FT3_DATASET_NAME)->SetValue(&uint8Tmp,1);
	uint16Tmp = BSWAP_16(asduPublic.ldName);
	m_AsduData->at(NANRUI_FT3_LD_NAME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.phsArtg);
	m_AsduData->at(NANRUI_FT3_PHS_ARTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.neutArtg);
	m_AsduData->at(NANRUI_FT3_NEUT_ARTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.phsVrtg);
	m_AsduData->at(NANRUI_FT3_PHS_VRTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.delayTime);
	m_AsduData->at(NANRUI_FT3_DELAY_TIME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.reserved);
	m_AsduData->at(NANRUI_FT3_RESERVED)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord1);
	m_AsduData->at(NANRUI_FT3_STATUS_WORD_1)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord2);
	m_AsduData->at(NANRUI_FT3_STATUS_WORD_2)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.smpCnt);
	m_AsduData->at(NANRUI_FT3_SMPCNT)->SetValue((uint8_t *)&uint16Tmp,2);
	uint8Tmp  = asduPublic.smpRate;
	m_AsduData->at(NANRUI_FT3_SMPRATE)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.confRev;
	m_AsduData->at(NANRUI_FT3_CONFREV)->SetValue(&uint8Tmp,1);

	// 设置数据通道
	int i = NANRUI_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= NANRUI_FT3_DATA_CHANNEL_12; i++ )
	{
		if ( i == NANRUI_FT3_CRC2 )
			continue;
		// 多余的通道，值置为0
		if ( smpNum >= bufSize )
			uint16Tmp = 0;
		else
			uint16Tmp = BSWAP_16(smpValBuf[smpNum]);
		m_AsduData->at(i)->SetValue((uint8_t *)&uint16Tmp,2);
		smpNum++;
	}
}
