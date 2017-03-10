#pragma once

#include <QList>
#include <QDomDocument>
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"
#include "XenaManager/src/dialog/modifierDialog/modifierDialog.h"
#include "port.h"

class CPort;
class CStream
{
public:
    CStream(CPort* chassis = NULL);
    ~CStream();

    enum Strategy {
        STRATEGY_NONE,      //  û�ж��Ʒ��Ͳ���
        STRATEGY_FRAMES,    //  ��֡�������ͣ����꼴ֹͣ����
        STRATEGY_STEP,      //  �������ʲ����ķ�ʽ����
    };

    enum SettingsType {
        SETTINGS_PPS,       //  ��ÿ�뷢��֡����������  Ĭ��
        SETTINGS_PERCENT    //  ���ٷֱ���������
    };

    CPort* port() const {return m_port;}
    int itemOfPort() const {return m_itemOfPort;}
    void setItemOfPort(int item) {m_itemOfPort = item;}
    void setPort(CPort* port_) {m_port = port_;}


    void dumpMsg();

    const QString & streamName() {return m_streamName;}
    void setStreamName(const QString &name) {m_streamName = name;}

    /*
     *  ������
     */
    int maxModifierCount();

    int modifierCount() {return m_modifierCnt;}
    void setModifierCount(quint32 modifierCnt) {m_modifierCnt = modifierCnt;}

    quint16 modifierDataCount() const {return m_modifierData.size();}
    bool appendModifierData(CModifierDialog::ModifierData * modifierData_);
    int findModifierData(CModifierDialog::ModifierData* modifierData_) const;
    int itemOfModiferData(const CModifierDialog::ModifierData & modifierData_);
    CModifierDialog::ModifierData* modifierData(quint16 item) const;
    /*
     *  @Func:  �Ƴ�������
     *  
     *  @Param[in]:
     *      modifierData_:  Ҫ�Ƴ�����������ݵ�ָ��
     *  
     *  @Return:
     *      CModifierDialog::ModifierData*:   �����Ƴ�������������ݼ���ָ�� 
     *      NULL    :   fail �޴�����������
     *  @Note:
     *      ��Ҫ���ⲿ�ͷŸ�CModifierDialog::ModifierData��Դ
     */
    CModifierDialog::ModifierData* removeModifierData(CModifierDialog::ModifierData *modifierData_);
    CModifierDialog::ModifierData* removeModifierData(int modifierDataIndex);

    /*
     *  ʹ�ܷ�����
     */
    bool isEnableTx() {return m_bEnableTx;}
    void enableTx(bool enable) {m_bEnableTx = enable;}

    /*
     *  ���÷�������
     *  @Note��
     *      1���˿��������ķ������ʳ���100% ����ʧ��
     *      2�����ʿ��԰��հٷֱȺ�ÿ�뷢��֡���趨����������һ�ַ�ʽ���ã�����һ������Ҳ�ᱻ�ı�
     */
    bool  setTxPercent(float percent);
    bool  setTxFramesPerSecond(int framesPerSecond);
    float txPercent() {return m_txPercent;}
    int   txFramesPerSecond() {return m_txFramesPerSecond;}
    float maxTxPercent();
    int   maxTxFramesPerSecond();

    /*
     *  ��������
     */
    const quint8* frames() {return m_data.frame;}
    const ImportData* const_data() {return &m_data;}
    ImportData* data() {return &m_data;}
    void setData(const ImportData &data_) {m_data = data_;}
    void setData(const ImportData *data_) {m_data = *data_;}

    float framesPerSecondToPercent(int framesPerSecond);
    int percentToFramesPerSecond(float percent);


    void setSettingsType(SettingsType newType) {
        if (m_settingsType != newType) {
            m_settingsType = newType;
        }
        settingsTypeConv(newType);
    }
    SettingsType settingsType() {return m_settingsType;}



    /************************************************************************/
    /* ���Խӿ�                                                             */
    /************************************************************************/
    /*
     *  ���ò��Է��������������ٷֱȵݱ�
     *  @Note��
     *      1��֧�ֵ����͵ݼ����ַ���
     */
    bool setStrategy(quint32 framesNum, float start, float end, float step, int loop);
    /*
     *  ���ò��Է���������ÿ�뷢�͵�֡���ݱ�
     *  @Note��
     *      1��֧�ֵ����͵ݼ����ַ���
     */
    bool setStrategy(quint32 framesNum, int start, int end, int step, int loop);
    /*
     *  ���ò��Է���������ָ��������֡
     */
    bool setStrategy(quint32 framesNum);
    /*
     *  ���ò��Է�����Ĭ�Ϸ��ͷ�ʽ
     */
    bool setStrategy();

    Strategy strategy() {return m_strategy;}
    void setStrategyType(const Strategy type) {m_strategy = type;}      // ֻ�ڸ���streamʱʹ��

    /*
     *  @Func:  nextPercent
     *  
     *  @Param[out]:
     *      ok:         boolָ�룬true:�ɹ���ȡ����һ֡�İٷֱȣ�false����ȡʧ��
     *  
     *  @Return:
     *      ��һ�����Ե�����ֵ �ٷֱ�
     *  
     *  @Note:
     *      ok����ʧ�ܵ�ԭ��
     *      1����ǰ���������÷�ʽ���ǰ���SETTINGS_PERCENT���趨
     *      2����ǰ�Ĳ��Բ���STRATEGY_STEP
     *      3����ǰ�Ĳ��Դﵽ��������
     */
    float nextPercent(bool *ok = NULL);
    /*
     *  @Func:  nextFramesPerSecond
     *  
     *  @Param[out]:
     *      ok:         boolָ�룬true:�ɹ���ȡ����һ֡��PPS��false����ȡʧ��
     *  
     *  @Return:
     *      ��һ�����Ե�����ֵ PPS
     *  
     *  @Note:
     *      ok����ʧ�ܵ�ԭ��
     *      1����ǰ���������÷�ʽ���ǰ���SETTINGS_PPS���趨
     *      2����ǰ�Ĳ��Բ���STRATEGY_STEP
     *      3����ǰ�Ĳ��Դﵽ��������
     */
    int   nextFramesPerSecond(bool *ok = NULL);

    void resetStrategyStatus();

    // ����set����ֻ�ڸ���streamʱʹ��
    float startPercent() {return m_startPercent;}
    void setStartPercent(const float percent_) {m_startPercent = percent_;}
    float endPercent() {return m_endPercent;}
    void setEndPercent(const float  percent_) {m_endPercent = percent_;}
    float stepPercent() {return m_stepPercent;}
    void setStepPercent(const float percent_) {m_stepPercent = percent_;}
    int   startFramesPerSec() {return m_startFramesPerSec;}
    void setStartFramesPerSec(const int startFramesPerSec_) {m_startFramesPerSec = startFramesPerSec_;}
    int   endFramesPerSec() {return m_endFramesPerSec;}
    void setEndFramesPerSec(const int endFramesPerSec_) {m_endFramesPerSec = endFramesPerSec_;}
    int   stepFramesPerSec() {return m_stepFramesPerSec;}
    void setStepFramesPerSec(const int stepFramesPerSec_) {m_stepFramesPerSec = stepFramesPerSec_;}
    int   loopCnt() {return m_loopCnt;}
    void setLoopCnt(const int loopCnt_) {m_loopCnt = loopCnt_;}
    int   packetLimit() {return m_txPacketsLimit;}    
    void setPacketLimit(const int packetLimie_) {m_txPacketsLimit = packetLimie_;}

    static bool toXml(const CStream *stream, QDomDocument &doc, QDomElement &streamDom);
    static  bool fromXml(QDomElement &streamDom, CStream &stream);

private:
    void settingsTypeConv(SettingsType newType);
    
private:
    CPort*  m_port;
    bool    m_bEnableTx;
    QString m_streamName;
    SettingsType m_settingsType;

    int     m_itemOfPort;                       //  ��xena�豸�е������

    ImportData m_data;                          //��������

    float       m_txPercent;                    //�������ʵİٷֱ�
    int         m_txFramesPerSecond;            //ÿ�뷢�Ͷ���֡����
    quint32     m_txPacketsLimit;

    //����
    Strategy m_strategy;

    float m_startPercent;
    float m_endPercent;
    float m_stepPercent;

    int m_startFramesPerSec;
    int m_endFramesPerSec;
    int m_stepFramesPerSec;

    int m_loopCnt;
    int m_currentLoopItem;

    // ������
    QList<CModifierDialog::ModifierData *> m_modifierData;
    quint32     m_modifierCnt;
};


inline float CStream::framesPerSecondToPercent(int framesPerSecond)
{
    Q_ASSERT(m_port);
    int bytes = framesPerSecond * (m_data.frameLength+4 + 20);   //4�ֽڵ�CRC
    return (float)(bytes*8*100.0f) / (float)(m_port->realSpeed() * 1000000);
}

inline int CStream::percentToFramesPerSecond(float percent)
{
    Q_ASSERT(m_port);
    int bytes = m_port->realSpeed() * 1000000 * percent /100.0f / 8;
    return bytes / (m_data.frameLength+4 + 20);                 //4�ֽڵ�CRC
}