#ifndef SAV91_PDU_SEG_H_

#define SAV91_PDU_SEG_H_

#include <vector>
#include "../include/sav91_types.h"

class Sav91AsduSeg;

class Sav91PduSeg
{
public:
	Sav91PduSeg();
	~Sav91PduSeg();

	typedef enum {SAV91_IEEE802_1Q = 0}SAV91_ULP_TYPE;

	size_t Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize);
	size_t Code(uint8_t *pduDataArr, const size_t arrSize);

	size_t GetPduLen();
	uint16_t GetNoASDU();
	bool   GetASDUPublicInfo(const size_t idxSeq, const uint8_t *msgDataBuf, SAV91_ASDU_PUBLIC &asduPublic);
	bool   GetSmpCntInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &smpCnt);
	bool   GetStatusWordsInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &statusWord1, uint16_t &statusWord2);
	size_t GetASDUSmpsNum();
	size_t GetASDUSmps(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize);

	size_t GetPduOff();
	size_t GetNoASDUOff();
	bool   GetASDUPublicInfoOff(const size_t idxSeq, SAV91_ASDU_PUBLIC_OFFSET &asduPublicOff);
	size_t GetASDUSmpsOff(const size_t idxSeq, size_t *smpOffArr, const size_t arrSize);

	void   Clear();
	void   SetULPType(const SAV91_ULP_TYPE type);
	bool   AddAnASDU(const SAV91_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize);

private:
	uint16_t m_NoASDU;
	size_t   m_NoASDUOff;
	size_t   m_PduLen;
	size_t   m_PduOff;
	size_t   m_AsduSize;
	size_t   m_MaxPduSize;
	std::vector<Sav91AsduSeg *> m_AsduSubSeg;
};

#endif
