#include "module.h"
#include "stream.h"
#include "port.h"

#include <QDebug>


CStream::CStream(CPort* port_/* = NULL*/)
: m_port(port_)
, m_bEnableTx(true)
, m_streamName("UNKNOWN")
, m_txPercent(10.0)
, m_txFramesPerSecond(0)
, m_txPacketsLimit(0)
, m_strategy(STRATEGY_NONE)
, m_loopCnt(1)
, m_settingsType(SETTINGS_PERCENT)
, m_startPercent(1.0)
, m_endPercent(100.0)
, m_stepPercent(1.0)
, m_startFramesPerSec(100)
, m_endFramesPerSec(1000)
, m_stepFramesPerSec(2)
, m_itemOfPort(-1)
{
}

CStream::~CStream()
{
    Q_FOREACH(CModifierDialog::ModifierData* s, m_modifierData)
    {
        if (s) {
            delete s;
        }
    }
}

void CStream::dumpMsg()
{
    qDebug( "++++++++++++  stream:%d  ++++++++++", itemOfPort());

    qDebug() << "streamName" << streamName();
    qDebug() << "isEnable:" << isEnableTx();
    qDebug() << "txPercent:" << txPercent();
    qDebug() << "txFramesPerSecond:" << txFramesPerSecond();
    qDebug() << "streamLength:" << m_data.frameLength;

    qDebug( "------------  stream:%d  ----------", itemOfPort());

}


int CStream::maxModifierCount()
{
    QString moduleName = port()->module()->name();

    int modiferCount;
    if (moduleName.contains("M6SFP",Qt::CaseInsensitive))
    {
        modiferCount = 2;
    }
    else if (moduleName.contains("M2SFPT",Qt::CaseInsensitive))
    {
        modiferCount = 6;
    }

    return modiferCount;
}

bool CStream::appendModifierData(CModifierDialog::ModifierData * modifierData_)
{
    if (!modifierData_) {
        return false;
    }

    m_modifierData.push_back(modifierData_);
    return true;
}

int CStream::findModifierData(CModifierDialog::ModifierData* modifierData_) const
{
    if (!modifierData_) {
        return -1;
    }

    int item = 0;
    Q_FOREACH(CModifierDialog::ModifierData* s, m_modifierData) {
        if (s == modifierData_) {
            return item;
        }
        item++;
    }

    return -1;
}

int CStream::itemOfModiferData(const CModifierDialog::ModifierData & modifierData_)
{
    for (int item = 0;item < m_modifierData.size();item++)
    {
        if (modifierData_.bitsNum == m_modifierData[item]->bitsNum
            && modifierData_.maxVal == m_modifierData[item]->maxVal
            && modifierData_.minVal == m_modifierData[item]->minVal
            && modifierData_.pos == m_modifierData[item]->pos
            && modifierData_.repeatCnt == m_modifierData[item]->repeatCnt
            && modifierData_.step == m_modifierData[item]->step
            && modifierData_.type == m_modifierData[item]->type)
        {
            return item;
        }
    }

    return -1;
}

CModifierDialog::ModifierData* CStream::modifierData(quint16 item) const
{
    if (item < m_modifierData.size()) {
        return m_modifierData.at(item);
    }
    return NULL;
}

CModifierDialog::ModifierData* CStream::removeModifierData(CModifierDialog::ModifierData *modifierData_)
{
    return removeModifierData(findModifierData(modifierData_));
}

CModifierDialog::ModifierData* CStream::removeModifierData(int modifierDataIndex)
{
    if (modifierDataIndex < 0 || modifierDataIndex >= modifierDataCount())
    {
        return NULL;
    }

    return m_modifierData.takeAt(modifierDataIndex);
}

bool CStream::setTxPercent(float percent)
{
    if (percent <= maxTxPercent()) {
        m_txPercent = percent;
        return true;
    }
    return false;
}

bool CStream::setTxFramesPerSecond(int framesPerSecond)
{
    if (framesPerSecond <= maxTxFramesPerSecond()) {
        m_txFramesPerSecond = framesPerSecond;
        return true;
    }
    return false;
}

float CStream::maxTxPercent()
{
    float percent = 0.00;
    if (m_port) {
        for(int i = 0; i < m_port->streamCount(); i++) {
            CStream* stream = m_port->stream(i);
            if (stream != this) {
                percent += stream->txPercent();
            }
        }
    }

    return 100.0f - percent;
}

int   CStream::maxTxFramesPerSecond()
{
    return percentToFramesPerSecond(maxTxPercent());
}


/************************************************************************/
/* 策略实现                                                             */
/************************************************************************/
bool CStream::setStrategy(quint32 framesNum, float start, float end, float step, int loop)
{
    if (m_settingsType != SETTINGS_PERCENT && !setTxPercent(start)) {
        return false;
    }
    if (step > 0.0 && start > end) {
        return false;
    }
    if (step < 0.0 && start < end) {
        return false;
    }
    if (step == 0.0) {
        return false;
    }

    if (loop == 0) {
        loop = 1;
    }

    m_txPacketsLimit = framesNum;
    m_startPercent = start;
    m_endPercent = end;
    m_stepPercent = step;
    m_loopCnt = loop;
    m_strategy = STRATEGY_STEP;
    m_currentLoopItem = 0;

    return true;
}

bool CStream::setStrategy(quint32 framesNum, int start, int end, int step, int loop)
{
    if (m_settingsType != SETTINGS_PPS && !setTxFramesPerSecond(start)) {
        return false;
    }
    if (step > 0 && start > end) {
        return false;
    }
    if (step < 0 && start < end) {
        return false;
    }
    if (step == 0) {
        return false;
    }

    if (loop == 0) {
        loop = 1;
    }

    m_txPacketsLimit = framesNum;
    m_startFramesPerSec = start;
    m_endFramesPerSec = end;
    m_stepFramesPerSec = step;
    m_loopCnt = loop;
    m_strategy = STRATEGY_STEP;
    m_currentLoopItem = 0;

    return true;
}

bool CStream::setStrategy(quint32 framesNum)
{
    m_txPacketsLimit = framesNum;
    m_strategy = STRATEGY_FRAMES;
    m_currentLoopItem = 0;
    return true;
}

bool CStream::setStrategy()
{
    m_strategy = STRATEGY_NONE;
    return true;
}

float CStream::nextPercent(bool *ok)
{
    if (m_settingsType != SETTINGS_PERCENT || m_txPercent < 0.0009) {
        goto end;
    }

    if (m_strategy == STRATEGY_STEP)
    {
        float percent = m_txPercent + m_stepPercent;
        if ((m_stepPercent > 0.0 && percent > m_endPercent) ||
            (m_stepPercent < 0.0 && percent < m_endPercent))
        {
            m_currentLoopItem++;
            if (m_currentLoopItem < m_loopCnt) {
                percent = m_startPercent;
            }
            else {
                m_txPercent = 0;
                goto end;
            }
        }
        if (ok) *ok = true;
        return percent;
    }

end:
    if (ok) *ok = false;
    return m_txPercent;
}
int   CStream::nextFramesPerSecond(bool *ok)
{
    if (m_settingsType != SETTINGS_PPS || m_txFramesPerSecond == 0) {
        goto end;
    }

    if (m_strategy == STRATEGY_STEP)
    {
        int percent = m_txFramesPerSecond + m_stepFramesPerSec;
        if ((m_stepFramesPerSec > 0 && percent > m_endFramesPerSec) ||
            (m_stepFramesPerSec < 0 && percent < m_endFramesPerSec))
        {
            m_currentLoopItem++;
            if (m_currentLoopItem < m_loopCnt) {
                percent = m_startFramesPerSec;
            }
            else {
                m_txFramesPerSecond = 0;
                goto end;
            }
        }
        if (ok) *ok = true;
        return percent;
    }

end:
    if (ok) *ok = false;
    return m_txFramesPerSecond;
}


void CStream::resetStrategyStatus()
{
    if (m_strategy == STRATEGY_STEP) {
        m_txFramesPerSecond = m_startFramesPerSec;
        m_txPercent = m_startPercent;
        m_currentLoopItem = 0;
    }
}

void CStream::settingsTypeConv(SettingsType newType)
{
    if (newType == SETTINGS_PERCENT) {
        m_txPercent = framesPerSecondToPercent(m_txFramesPerSecond);
        m_startPercent = framesPerSecondToPercent(m_startFramesPerSec);
        m_endPercent = framesPerSecondToPercent(m_endFramesPerSec);
        m_stepPercent = framesPerSecondToPercent(m_stepFramesPerSec);
    }
    else {
        m_txFramesPerSecond = percentToFramesPerSecond(m_txPercent);
        m_startFramesPerSec = percentToFramesPerSecond(m_startPercent);
        m_endFramesPerSec = percentToFramesPerSecond(m_endPercent);
        m_stepFramesPerSec = percentToFramesPerSecond(m_stepPercent);
    }
}


bool CStream::toXml(const CStream *stream, QDomDocument &doc, QDomElement &streamDom)
{
    QDomElement child = doc.createElement("enable");
    child.setAttribute(QString("val"), stream->m_bEnableTx);
    streamDom.appendChild(child);

    child = doc.createElement("name");
    child.setAttribute(QString("val"), stream->m_streamName);
    streamDom.appendChild(child);

    child = doc.createElement("index");                                 // 在xena中的索引
    child.setAttribute(QString("val"), stream->m_itemOfPort);
    streamDom.appendChild(child);

    child = doc.createElement("SettingsType");
    if (stream->m_settingsType == SETTINGS_PERCENT) 
        child.setAttribute(QString("val"), "percent");
    else 
        child.setAttribute(QString("val"), "pps");
    streamDom.appendChild(child);

    child = doc.createElement("txPercent");
    child.setAttribute(QString("val"), stream->m_txPercent);
    streamDom.appendChild(child);
    child = doc.createElement("txFramesPerSecond");
    child.setAttribute(QString("val"), stream->m_txFramesPerSecond);
    streamDom.appendChild(child);
    child = doc.createElement("txPacketsLimit");
    child.setAttribute(QString("val"), stream->m_txPacketsLimit);
    streamDom.appendChild(child);

    child = doc.createElement("strategy");
    if (stream->m_strategy == STRATEGY_NONE) 
        child.setAttribute(QString("val"), "none");
    else if(stream->m_strategy == STRATEGY_FRAMES) 
        child.setAttribute(QString("val"), "frames");
    else
        child.setAttribute(QString("val"), "step");
    streamDom.appendChild(child);
    child = doc.createElement("startPercent");
    child.setAttribute(QString("val"), stream->m_startPercent);
    streamDom.appendChild(child);
    child = doc.createElement("endPercent");
    child.setAttribute(QString("val"), stream->m_endPercent);
    streamDom.appendChild(child);
    child = doc.createElement("stepPercent");
    child.setAttribute(QString("val"), stream->m_stepPercent);
    streamDom.appendChild(child);
    child = doc.createElement("startFramesPerSec");
    child.setAttribute(QString("val"), stream->m_startFramesPerSec);
    streamDom.appendChild(child);
    child = doc.createElement("endFramesPerSec");
    child.setAttribute(QString("val"), stream->m_endFramesPerSec);
    streamDom.appendChild(child);
    child = doc.createElement("stepFramesPerSec");
    child.setAttribute(QString("val"), stream->m_stepFramesPerSec);
    streamDom.appendChild(child);
    child = doc.createElement("loopCnt");
    child.setAttribute(QString("val"), stream->m_loopCnt);
    streamDom.appendChild(child);

    child = doc.createElement("ModifierDatas");
    Q_FOREACH( CModifierDialog::ModifierData *data, stream->m_modifierData)
    {
        QDomElement modifier = doc.createElement("ModifierData");

        QDomElement ele = doc.createElement("pos");
        ele.setAttribute(QString("val"), data->pos);
        modifier.appendChild(ele);
        ele = doc.createElement("minVal");
        ele.setAttribute(QString("val"), data->minVal);
        modifier.appendChild(ele);
        ele = doc.createElement("maxVal");
        ele.setAttribute(QString("val"), data->maxVal);
        modifier.appendChild(ele);
        ele = doc.createElement("step");
        ele.setAttribute(QString("val"), data->step);
        modifier.appendChild(ele);
        ele = doc.createElement("repeatCnt");
        ele.setAttribute(QString("val"), data->repeatCnt);
        modifier.appendChild(ele);
        ele = doc.createElement("type");
        if (data->type == CModifierDialog::INCREMENT)
            ele.setAttribute(QString("val"), "increment");
        else if(data->type == CModifierDialog::DECREMENT)
            ele.setAttribute(QString("val"), "decrement");
        else 
            ele.setAttribute(QString("val"), "random");
        modifier.appendChild(ele);
        ele = doc.createElement("bitsNum");
        ele.setAttribute(QString("val"), data->bitsNum);
        modifier.appendChild(ele);

        child.appendChild(modifier);
    }
    streamDom.appendChild(child);

    child = doc.createElement("Data");
    {
        QDomElement ele = doc.createElement("frameStyle");
        ele.setAttribute(QString("val"), stream->m_data.frameStyle);
        child.appendChild(ele);
        ele = doc.createElement("appId");
        ele.setAttribute(QString("val"), QString("%1").arg(stream->m_data.appId, 4, 16, QChar('0')));
        child.appendChild(ele);
        ele = doc.createElement("iedName");
        ele.setAttribute(QString("val"), stream->m_data.iedName);
        child.appendChild(ele);

        QString frame;
        for(quint16 i = 0; i < stream->m_data.frameLength; i++) {
            frame.append(QString("%1").arg(stream->m_data.frame[i], 2, 16, QChar('0')));
        }
        ele = doc.createElement("frame");
        ele.setAttribute(QString("val"), frame);
        child.appendChild(ele);
    }
    streamDom.appendChild(child);

    return true;
}

static bool xmlToModifier(QDomElement &modifier, CModifierDialog::ModifierData *data)
{
    QDomElement modifierEle = modifier.firstChildElement(QString("pos"));
    data->pos = modifierEle.attribute(QString("val")).toUInt();
    modifierEle = modifier.firstChildElement(QString("minVal"));
    data->minVal = modifierEle.attribute(QString("val")).toUInt();
    modifierEle = modifier.firstChildElement(QString("maxVal"));
    data->maxVal = modifierEle.attribute(QString("val")).toUInt();
    modifierEle = modifier.firstChildElement(QString("step"));
    data->step = modifierEle.attribute(QString("val")).toUInt();
    modifierEle = modifier.firstChildElement(QString("repeatCnt"));
    data->repeatCnt = modifierEle.attribute(QString("val")).toUInt();
    modifierEle = modifier.firstChildElement(QString("type"));
    {
        QString type = modifierEle.attribute(QString("val"));
        if (type == "increment")
            data->type = CModifierDialog::INCREMENT;
        else if (type == "decrement")
            data->type = CModifierDialog::DECREMENT;
        else if (type == "random")
            data->type = CModifierDialog::RANDOM;
        else 
            return false;
    }
    modifierEle = modifier.firstChildElement(QString("bitsNum"));
    {
        QString bitsNum = modifierEle.attribute(QString("val"));
        if (bitsNum == "0")
            data->bitsNum = CModifierDialog::MODIFY_8BITS;
        else if (bitsNum == "1")
            data->bitsNum = CModifierDialog::MODIFY_16BITS;
        else 
            return false;
    }

    return true;
}

bool CStream::fromXml(QDomElement &streamDom, CStream &stream)
{
    try
    {
        QDomElement ele = streamDom.firstChildElement(QString("enable"));
        stream.m_bEnableTx = ele.attribute(QString("val"), QString("0")).toInt() ? true : false;

        ele = streamDom.firstChildElement(QString("name"));
        stream.m_streamName = ele.attribute(QString("val"));

        ele = streamDom.firstChildElement(QString("index"));
        stream.m_itemOfPort = ele.attribute(QString("val")).toInt();

        ele = streamDom.firstChildElement(QString("SettingsType"));
        {
            QString type = ele.attribute(QString("val"));
            if (type == "percent")
                stream.m_settingsType = SETTINGS_PERCENT;
            else if (type == "pps")
                stream.m_settingsType = SETTINGS_PPS;
            else 
                return false;
        }
        ele = streamDom.firstChildElement(QString("txPercent"));
        stream.m_txPercent = ele.attribute(QString("val")).toDouble();
        ele = streamDom.firstChildElement(QString("txFramesPerSecond"));
        stream.m_txFramesPerSecond = ele.attribute(QString("val")).toInt();
        ele = streamDom.firstChildElement(QString("txPacketsLimit"));
        stream.m_txPacketsLimit = ele.attribute(QString("val")).toUInt();
        ele = streamDom.firstChildElement(QString("strategy"));
        {
            QString strategy = ele.attribute(QString("val"));
            if (strategy == "none")
                stream.m_strategy = STRATEGY_NONE;
            else if (strategy == "frames")
                stream.m_strategy = STRATEGY_FRAMES;
            else if (strategy == "step")
                stream.m_strategy = STRATEGY_STEP;
            else 
                return false;
        }
        ele = streamDom.firstChildElement(QString("startPercent"));
        stream.m_startPercent = ele.attribute(QString("val")).toDouble();
        ele = streamDom.firstChildElement(QString("endPercent"));
        stream.m_endPercent = ele.attribute(QString("val")).toDouble();
        ele = streamDom.firstChildElement(QString("stepPercent"));
        stream.m_stepPercent = ele.attribute(QString("val")).toDouble();
        ele = streamDom.firstChildElement(QString("startFramesPerSec"));
        stream.m_startFramesPerSec = ele.attribute(QString("val")).toInt();
        ele = streamDom.firstChildElement(QString("endFramesPerSec"));
        stream.m_endFramesPerSec = ele.attribute(QString("val")).toInt();
        ele = streamDom.firstChildElement(QString("stepFramesPerSec"));
        stream.m_stepFramesPerSec = ele.attribute(QString("val")).toInt();
        ele = streamDom.firstChildElement(QString("loopCnt"));
        stream.m_loopCnt = ele.attribute(QString("val")).toInt();

        ele = streamDom.firstChildElement(QString("ModifierDatas"));
        {
            QDomElement modifier = ele.firstChildElement(QString("ModifierData"));
            stream.m_modifierData.clear();
            while(!modifier.isNull()) {
                CModifierDialog::ModifierData *data = new CModifierDialog::ModifierData();
                if (xmlToModifier(modifier, data)) {
                    stream.m_modifierData.append(data);
                }
                else {
                    return false;
                }
                modifier = modifier.nextSiblingElement();
            }
        }

        ele = streamDom.firstChildElement(QString("Data"));
        {
            QDomElement dataEle = ele.firstChildElement(QString("frameStyle"));
            stream.m_data.frameStyle = dataEle.attribute(QString("val"));

            dataEle = ele.firstChildElement(QString("appId"));
            stream.m_data.appId = dataEle.attribute(QString("val")).toUInt(0, 16);

            dataEle = ele.firstChildElement(QString("iedName"));
            stream.m_data.iedName = dataEle.attribute(QString("val"));

            dataEle = ele.firstChildElement(QString("frame"));
            QString frame = dataEle.attribute(QString("val"));

            stream.m_data.frameLength = frame.length() / 2;
            for(int i = 0; i < stream.m_data.frameLength; i++) {
                QString data = frame.mid(i*2, 2);
                stream.m_data.frame[i] = data.toUInt(0, 16);
            }
        }

    }
    catch (...)
    {
        qDebug() << "CStream::fromXml detect excpetion";
        return false;
    }

    return true;
}