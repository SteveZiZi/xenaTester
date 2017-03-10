#pragma once

#include <QList>
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
    int itemOfPort() const;
    void setPort(CPort* port_) {m_port = port_;}


    void dumpMsg();

    const QString & streamName() {return m_streamName;}
    void setStreamName(const QString &name) {m_streamName = name;}

    /*
     *  跳变域
     */
    int maxModifierCount();

    quint16 modifierDataCount() const {return m_modifierData.size();}
    bool appendModifierData(CModifierDialog::ModifierData * modifierData_);
    int findModifierData(CModifierDialog::ModifierData* modifierData_) const;
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
     *      需要在外部释放该stream资源
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
    int streamLength() {return m_data.frameLength;}
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

    float startPercent() {return m_startPercent;}
    float endPercent() {return m_endPercent;}
    float stepPercent() {return m_stepPercent;}
    int   startFramesPerSec() {return m_startFramesPerSec;}
    int   endFramesPerSec() {return m_endFramesPerSec;}
    int   stepFramesPerSec() {return m_stepFramesPerSec;}
    int   loopCnt() {return m_loopCnt;}
    int   packetLimit() {return m_txPacketsLimit;}    

private:
    void settingsTypeConv(SettingsType newType);
    
private:
    CPort*  m_port;
    bool    m_bEnableTx;
    QString m_streamName;
    SettingsType m_settingsType;

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
};


inline float CStream::framesPerSecondToPercent(int framesPerSecond)
{
    Q_ASSERT(m_port);
    int bytes = framesPerSecond * (m_data.frameLength + 20);
    return (float)(bytes*8*100.0f) / (float)(m_port->realSpeed() * 1000000);
}

inline int CStream::percentToFramesPerSecond(float percent)
{
    Q_ASSERT(m_port);
    int bytes = m_port->realSpeed() * 1000000 * percent /100.0f / 8;
    return bytes / (m_data.frameLength + 20);
}