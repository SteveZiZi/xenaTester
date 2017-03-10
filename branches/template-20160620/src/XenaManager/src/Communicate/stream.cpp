#include "module.h"
#include "stream.h"
#include "port.h"

#include <QDebug>


CStream::CStream(CPort* port_/* = NULL*/)
: m_port(port_)
, m_bEnableTx(true)
, m_streamName("UNKNOWN")
, m_txPercent(0.0)
, m_txFramesPerSecond(0)
, m_txPacketsLimit(0)
, m_strategy(STRATEGY_NONE)
, m_loopCnt(0)
, m_settingsType(SETTINGS_PPS)
, m_startPercent(1.0)
, m_endPercent(100.0)
, m_stepPercent(2.0)
, m_startFramesPerSec(100)
, m_endFramesPerSec(1000)
, m_stepFramesPerSec(2)
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


int CStream::itemOfPort() const
{
    if (m_port) 
        return m_port->findStream(this);

    return -1;
}



void CStream::dumpMsg()
{
    qDebug( "++++++++++++  stream:%d  ++++++++++", itemOfPort());

    qDebug() << "streamName" << streamName();
    qDebug() << "isEnable:" << isEnableTx();
    qDebug() << "txPercent:" << txPercent();
    qDebug() << "txFramesPerSecond:" << txFramesPerSecond();
    qDebug() << "streamLength:" << streamLength();

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
/* ²ßÂÔÊµÏÖ                                                             */
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
    if (m_settingsType != SETTINGS_PERCENT || m_txPercent == 0.0) {
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

