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
        STRATEGY_NONE,      //  没有定制发送策略
        STRATEGY_FRAMES,    //  按帧数量发送，发完即停止发送
        STRATEGY_STEP,      //  按照速率步进的方式发送
    };

    enum SettingsType {
        SETTINGS_PPS,       //  按每秒发送帧数设置流量  默认
        SETTINGS_PERCENT    //  按百分比设置流量
    };

    CPort* port() const {return m_port;}
    int itemOfPort() const {return m_itemOfPort;}
    void setItemOfPort(int item) {m_itemOfPort = item;}
    void setPort(CPort* port_) {m_port = port_;}


    void dumpMsg();

    const QString & streamName() {return m_streamName;}
    void setStreamName(const QString &name) {m_streamName = name;}

    /*
     *  跳变域
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
     *  @Func:  移除跳变域
     *  
     *  @Param[in]:
     *      modifierData_:  要移除跳变域的数据的指针
     *  
     *  @Return:
     *      CModifierDialog::ModifierData*:   返回移除的跳变域的数据集的指针 
     *      NULL    :   fail 无此跳变域数据
     *  @Note:
     *      需要在外部释放该CModifierDialog::ModifierData资源
     */
    CModifierDialog::ModifierData* removeModifierData(CModifierDialog::ModifierData *modifierData_);
    CModifierDialog::ModifierData* removeModifierData(int modifierDataIndex);

    /*
     *  使能发送流
     */
    bool isEnableTx() {return m_bEnableTx;}
    void enableTx(bool enable) {m_bEnableTx = enable;}

    /*
     *  设置发送速率
     *  @Note：
     *      1、端口所有流的发送速率超过100% 返回失败
     *      2、速率可以按照百分比和每秒发送帧数设定，按照任意一种方式设置，另外一种设置也会被改变
     */
    bool  setTxPercent(float percent);
    bool  setTxFramesPerSecond(int framesPerSecond);
    float txPercent() {return m_txPercent;}
    int   txFramesPerSecond() {return m_txFramesPerSecond;}
    float maxTxPercent();
    int   maxTxFramesPerSecond();

    /*
     *  报文内容
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
    /* 策略接口                                                             */
    /************************************************************************/
    /*
     *  设置策略方案：按照流量百分比递变
     *  @Note：
     *      1、支持递增和递减两种方案
     */
    bool setStrategy(quint32 framesNum, float start, float end, float step, int loop);
    /*
     *  设置策略方案：按照每秒发送的帧数递变
     *  @Note：
     *      1、支持递增和递减两种方案
     */
    bool setStrategy(quint32 framesNum, int start, int end, int step, int loop);
    /*
     *  设置策略方案：发送指定数量的帧
     */
    bool setStrategy(quint32 framesNum);
    /*
     *  设置策略方案：默认发送方式
     */
    bool setStrategy();

    Strategy strategy() {return m_strategy;}
    void setStrategyType(const Strategy type) {m_strategy = type;}      // 只在复制stream时使用

    /*
     *  @Func:  nextPercent
     *  
     *  @Param[out]:
     *      ok:         bool指针，true:成功获取到下一帧的百分比，false：获取失败
     *  
     *  @Return:
     *      下一个策略的流量值 百分比
     *  
     *  @Note:
     *      ok返回失败的原因：
     *      1、当前流量的配置方式不是按照SETTINGS_PERCENT来设定
     *      2、当前的策略不是STRATEGY_STEP
     *      3、当前的策略达到结束条件
     */
    float nextPercent(bool *ok = NULL);
    /*
     *  @Func:  nextFramesPerSecond
     *  
     *  @Param[out]:
     *      ok:         bool指针，true:成功获取到下一帧的PPS，false：获取失败
     *  
     *  @Return:
     *      下一个策略的流量值 PPS
     *  
     *  @Note:
     *      ok返回失败的原因：
     *      1、当前流量的配置方式不是按照SETTINGS_PPS来设定
     *      2、当前的策略不是STRATEGY_STEP
     *      3、当前的策略达到结束条件
     */
    int   nextFramesPerSecond(bool *ok = NULL);

    void resetStrategyStatus();

    // 以下set函数只在复制stream时使用
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

    int     m_itemOfPort;                       //  在xena设备中的流序号

    ImportData m_data;                          //报文数据

    float       m_txPercent;                    //发送速率的百分比
    int         m_txFramesPerSecond;            //每秒发送多少帧报文
    quint32     m_txPacketsLimit;

    //策略
    Strategy m_strategy;

    float m_startPercent;
    float m_endPercent;
    float m_stepPercent;

    int m_startFramesPerSec;
    int m_endFramesPerSec;
    int m_stepFramesPerSec;

    int m_loopCnt;
    int m_currentLoopItem;

    // 跳变域
    QList<CModifierDialog::ModifierData *> m_modifierData;
    quint32     m_modifierCnt;
};


inline float CStream::framesPerSecondToPercent(int framesPerSecond)
{
    Q_ASSERT(m_port);
    int bytes = framesPerSecond * (m_data.frameLength+4 + 20);   //4字节的CRC
    return (float)(bytes*8*100.0f) / (float)(m_port->realSpeed() * 1000000);
}

inline int CStream::percentToFramesPerSecond(float percent)
{
    Q_ASSERT(m_port);
    int bytes = m_port->realSpeed() * 1000000 * percent /100.0f / 8;
    return bytes / (m_data.frameLength+4 + 20);                 //4字节的CRC
}