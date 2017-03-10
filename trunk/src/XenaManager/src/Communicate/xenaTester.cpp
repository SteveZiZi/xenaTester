#include "xenaTester.h"
#include "device.h"
#include "XenaManager/src/XenaManageSettings/loginsettings.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>

CXenaTester::CXenaTester()
{
    m_commDev = GetCommDevice();
	m_bLogin = false;
}

CXenaTester::~CXenaTester()
{

}

bool  CXenaTester::login(const QString &ip, int port, const QString &userName, const QString &pwd)
{
    m_bLogin = m_commDev->login(ip, port, userName, pwd);

    return m_bLogin;
}

void  CXenaTester::logout()
{
    if (m_bLogin) {
        m_commDev->logout();
        m_bLogin = false;
    }
}

bool CXenaTester::keepAlive() 
{
    return m_commDev->keepAlive();
}

bool  CXenaTester::queryChassisInfo()
{
#if 1
    m_commDev->acquireChassisInfo(&m_chassis);
    m_commDev->acquireAllModuleInfo(&m_chassis);
    m_commDev->acquireAllPortInfo(&m_chassis);
    //m_commDev->acquireAllPortConfig(&m_chassis);
    m_commDev->acquireAllPortFullConfig(&m_chassis);

    m_commDev->startMonitorChassisStatus(&m_chassis);

    return true;
#else
    bool rc = true;
    QString str;
    m_commDev->chassisName(str);
    m_chassis.setChassisName(str);
    m_commDev->chassisSerialNo(str);
    m_chassis.setChassisSerialNo(str);

    int mCnt = m_commDev->moduleCount();
    if (mCnt == -1) {
        return false;
    }
    for(int m = 0; m < mCnt; m++) {
        CModule* module = new CModule();
        if (m_commDev->moduleName(m, str))
        {
            module->setName(str);
            int pCnt =m_commDev->portCount(m);
            for(int p = 0; p < pCnt; p++) {
                CPort* port = new CPort();
                if (m_commDev->portName(m, p, str))
                {
                    port->setPortName(str);
                    port->setReserved(m_commDev->portReservedOwner(m, p, str));
                    port->setReservedBy(str);
                    bool link, active;
                    m_commDev->portCommunicationStatus(m, p, link, active);
                    port->setLinked(link);
                    port->setActived(active);
                    int tmp;
                    m_commDev->portMaxHeaderLength(m, p, tmp);
                    port->setMaxHeaderLength(tmp);
                    m_commDev->portTimeLimit(m, p, tmp);
                    port->setTxTimeLimit(tmp);
                    m_commDev->portRealSpeed(m, p, tmp);
                    port->setRealSpeed(tmp);
                }
                module->appendPort(port);
            }
        }
        m_chassis.appendModule(module);
    }
    return rc;
#endif
}


/************************************************************************/
/* 机箱操作                                                             */
/************************************************************************/
const QString & CXenaTester::chassisSerialNo()
{
    Q_ASSERT(m_bLogin);
    return m_chassis.chassisSerialNo();
}

bool CXenaTester::reserveChassis()
{
    if (m_commDev->reserveChassis())
    {
        m_chassis.setReserved(true);
        m_chassis.setReservedBy(QString(LoginSettings::getUserName().data()));
        return true;
    }

    return false;
}

bool CXenaTester::releaseChassis()
{
    if (m_commDev->releaseChassis())
    {
        m_chassis.setReserved(false);
        m_chassis.setReservedBy("");
        return true;
    }

    return false;
}

bool CXenaTester::relinquishChassis()
{
    if (m_commDev->relinquishChassis())
    {
        m_chassis.setReserved(false);
        m_chassis.setReservedBy("");
        return true;
    }

    return false;
}

bool CXenaTester::chassisReservedOwner(QString &owner)
{
    owner = m_chassis.reservedBy();
    return true;
}

/************************************************************************/
/* 板卡操作                                                             */
/************************************************************************/
bool CXenaTester::reserveModule(int moduleIndex)
{
    if (m_commDev->reserveModule(moduleIndex))
    {
        m_chassis.module(moduleIndex)->setReserved(true);
        m_chassis.module(moduleIndex)->setReservedBy(QString(LoginSettings::getUserName().data()));
        return true;
    }

    return false;
}

bool CXenaTester::releaseModule(int moduleIndex)
{
    if (m_commDev->releaseModule(moduleIndex))
    {
        m_chassis.module(moduleIndex)->setReserved(false);
        m_chassis.module(moduleIndex)->setReservedBy("");
        return true;
    }

    return false;
}

bool CXenaTester::relinquishModule(int moduleIndex)
{
    if (m_commDev->relinquishModule(moduleIndex))
    {
        m_chassis.module(moduleIndex)->setReserved(false);
        m_chassis.module(moduleIndex)->setReservedBy("");
        return true;
    }

    return false;
}

bool CXenaTester::moduleReservedOwner(int modelIndex, QString &owner)
{
    owner = m_chassis.module(modelIndex)->reservedBy();
    return true;
}

/************************************************************************/
/* 端口操作                                                             */
/************************************************************************/
bool CXenaTester::reservePort(int moduleIndex, int portIndex)
{
    if (m_commDev->reservePort(moduleIndex, portIndex))
    {
        CPort* port = m_chassis.port(moduleIndex, portIndex);
        port->setReserved(true);
        port->setReservedBy(QString(LoginSettings::getUserName().data()));
        return true;
    }

    return false;
}

bool CXenaTester::releasePort(int moduleIndex, int portIndex)
{
    if (m_commDev->releasePort(moduleIndex,portIndex))
    {
        CPort* port = m_chassis.port(moduleIndex, portIndex);
        port->setReserved(false);
        port->setReservedBy("");
        return true;
    }

    return false;
}

bool CXenaTester::relinquishPort(int moduleIndex,int portIndex)
{
    if (m_commDev->relinquishPort(moduleIndex,portIndex))
    {
        CPort* port = m_chassis.port(moduleIndex, portIndex);
        port->setReserved(false);
        port->setReservedBy("");
        return true;
    }

    return false;
}

bool CXenaTester::portReservedOwner(int moduleIndex, int portIndex, QString &owner)
{
    owner = m_chassis.port(moduleIndex, portIndex)->reservedBy();
    return true;
}

bool CXenaTester::portName(int moduleIndex, int portIndex, QString &name, QString &desc)
{
    name = m_chassis.port(moduleIndex, portIndex)->portName();
    return true;
}

bool CXenaTester::portCommunicationStatus(int moduleIndex, int portIndex, bool &linked, bool &actived)
{
    linked = m_chassis.port(moduleIndex, portIndex)->linked();
    actived = m_chassis.port(moduleIndex, portIndex)->actived();
    return true;
}

bool CXenaTester::addStream(int moduleIndex, int portIndex, const ImportData *data)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    if (!port || port->streamCount() >= port->maxStreamNum()) {
        return false;
    }
    
    CStream* stream = new CStream(port);
    stream->setData(data);
    if (data->frameStyle.contains("SMV92", Qt::CaseInsensitive)) {
        stream->setTxFramesPerSecond(4000);
    }
    else {
        stream->setTxFramesPerSecond(1);
    }
    QString name = QString("Stream%1 %2[%3]").arg(port->unUsedIndexOfStream())
                                             .arg(data->frameStyle)
                                             .arg(data->appId, 4, 16, QChar('0'));
    if (data->iedName != "UNKNOWN") {
        name += " ";
        QString destMac = QString("%1-%2-%3-%4-%5-%6")
                             .arg(data->frame[0], 2, 16, QChar('0'))
                             .arg(data->frame[1], 2, 16, QChar('0'))
                             .arg(data->frame[2], 2, 16, QChar('0'))
                             .arg(data->frame[3], 2, 16, QChar('0'))
                             .arg(data->frame[4], 2, 16, QChar('0'))
                             .arg(data->frame[5], 2, 16, QChar('0'));
        name += destMac.toUpper();
    }
    stream->setStreamName(name);
    stream->setItemOfPort(port->unUsedIndexOfStream());

    if (m_commDev->addStream(moduleIndex, portIndex, stream)) {
        port->appendStream(stream);
        return true;
    }

    delete stream;
    return false;
}

bool CXenaTester::addStream(int moduleIndex, int portIndex, int streamIndex, const ImportData *data)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    if (!port || port->streamCount() >= port->maxStreamNum()) {
        return false;
    }

    CStream* stream = new CStream(port);
    stream->setData(*data);
    if (data->frameStyle.contains("SMV92", Qt::CaseInsensitive)) {
        stream->setTxFramesPerSecond(4000);
    }
    else {
        stream->setTxFramesPerSecond(1);
    }
    QString name = QString("Stream%1 %2[%3]").arg(port->unUsedIndexOfStream())
        .arg(data->frameStyle)
        .arg(data->appId, 4, 16, QChar('0'));
    if (data->iedName != "UNKNOWN") {
        name += " ";
        QString destMac = QString("%1-%2-%3-%4-%5-%6")
            .arg(data->frame[0], 2, 16, QChar('0'))
            .arg(data->frame[1], 2, 16, QChar('0'))
            .arg(data->frame[2], 2, 16, QChar('0'))
            .arg(data->frame[3], 2, 16, QChar('0'))
            .arg(data->frame[4], 2, 16, QChar('0'))
            .arg(data->frame[5], 2, 16, QChar('0'));
        name += destMac.toUpper();
    }
    stream->setStreamName(name);
    stream->setItemOfPort(streamIndex);

    if (m_commDev->addStream(moduleIndex, portIndex, stream)) {
        port->appendStream(stream);
        return true;
    }

    delete stream;
    return false;
}

bool CXenaTester::addStream(int moduleIndex, int portIndex, CStream *stream)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    if (!port || port->streamCount() >= port->maxStreamNum()) {
        return false;
    }

    if (m_commDev->addStream(moduleIndex, portIndex, stream)) {
        port->appendStream(stream);
        return true;
    }

    return false;
}

bool CXenaTester::delStream(int moduleIndex, int portIndex, CStream *stream)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);

    if (!port) {
        return false;
    }

    if (m_commDev->delStream(moduleIndex,portIndex,stream)) {
        port->removeStream(stream);
        return true;
    }

    return false;
}

bool CXenaTester::getPortMaxHeaderLength(int moduleIndex, int portIndex, int &maxLen)
{
    if (!m_commDev->portMaxHeaderLength(moduleIndex,portIndex,maxLen))
    {
        return false;
    }

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setMaxHeaderLength(maxLen);

    return true;
}

void CXenaTester::setPortMaxHeaderLength(int moduleIndex, int portIndex, int maxLen)
{
    m_commDev->setPortMaxHeaderLength(moduleIndex,portIndex,maxLen);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setMaxHeaderLength(maxLen);
}

bool CXenaTester::getPortTimeLimit(int moduleIndex, int portIndex, int &limitMs)
{
    if (!m_commDev->portTimeLimit(moduleIndex,portIndex,limitMs))
    {
        return false;
    }

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setTxTimeLimit(limitMs);

    return true;
}

void CXenaTester::setPortTimeLimit(int moduleIndex, int portIndex, int limitMS)
{
    m_commDev->setPortTimeLimit(moduleIndex,portIndex,limitMS);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setTxTimeLimit(limitMS);
}

//  端口是否正在发送数据
bool CXenaTester::portIsTrafficing(int moduleIndex, int portIndex)
{
    int bps, pps;
    if (m_commDev->portTxStatistics(moduleIndex, portIndex, bps, pps)) {
        return (0 != bps || 0 != pps);
    }

    return false;
}
bool CXenaTester::portIsTrafficing(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portIsTrafficing(moduleIndex, portIndex);
}


bool CXenaTester::portStartSpeedTraffic(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    m_commDev->setPortTimeLimit(moduleIndex, portIndex, port->txTimeLimit());
    for(int i = 0; i < port->streamCount(); i++) {
        CStream*stream = port->stream(i);
        if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
            m_commDev->setStreamRate(moduleIndex, portIndex, i, stream->txPercent());
        }
        else {
            m_commDev->setStreamRate(moduleIndex, portIndex, i, stream->txFramesPerSecond());
        }
        int packetLimit = 0;
        if (stream->strategy() != CStream::STRATEGY_NONE) {
            packetLimit = stream->packetLimit();
        }
        m_commDev->setStreamPacketLimit(moduleIndex, portIndex, i, packetLimit);
        //  暂时不重新设置跳变域
    }

    bool traffic = true;
    m_commDev->setPortTraffic(moduleIndex, portIndex, traffic);
    port->setActived(traffic);

    return true;
}
bool CXenaTester::portStartSpeedTraffic(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portStartSpeedTraffic(moduleIndex, portIndex);
}

bool CXenaTester::portNextSpeedTraffic(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    m_commDev->setPortTimeLimit(moduleIndex, portIndex, port->txTimeLimit());
    int txStream = 0;
    for(int i = 0; i < port->streamCount(); i++) {
        CStream*stream = port->stream(i);
        bool ok = false;
        if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
            float percent = stream->nextPercent(&ok);
            if (!stream->setTxPercent(percent)) {
                QString info = QString("%1%2 %3%4 %5").arg(QObject::tr("module"))
                                                      .arg(moduleIndex)
                                                      .arg(QObject::tr("port"))
                                                      .arg(portIndex)
                                                      .arg(QObject::tr("arrived max flow"));
                QMessageBox::information(0, QObject::tr("Error"), info);
                return false;
            }
        }
        else {
            int frames = stream->nextFramesPerSecond(&ok);
            if (!stream->setTxFramesPerSecond(frames)) {
                QString info = QString("%1%2 %3%4 %5").arg(QObject::tr("module"))
                                                      .arg(moduleIndex)
                                                      .arg(QObject::tr("port"))
                                                      .arg(portIndex)
                                                      .arg(QObject::tr("arrived max flow"));
                QMessageBox::information(0, QObject::tr("Error"), info);
                return false;
            }
        }
        if (ok) txStream++;
        //  暂时不重新设置跳变域
    }

    return (txStream != 0) ? true : false;
}
bool CXenaTester::portNextSpeedTraffic(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portNextSpeedTraffic(moduleIndex, portIndex);
}

bool CXenaTester::portStopTranffic(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    bool traffic = false;
    m_commDev->setPortTraffic(moduleIndex, portIndex, traffic);
    port->setActived(traffic);

    return true;
}
bool CXenaTester::portStopTranffic(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portStopTranffic(moduleIndex, portIndex);
}


//  端口完成策略发送
bool CXenaTester::portFinishedStrategy(int moduleIndex, int portIndex)
{
    CPort* port = m_chassis.port(moduleIndex, portIndex);
    for(int i = 0; i < port->streamCount(); i++) {
        CStream*stream = port->stream(i);
        stream->resetStrategyStatus();
    }
    return true;
}

bool CXenaTester::portFinishedStrategy(CPort* port)
{
    int moduleIndex = port->module()->itemOfChassis();
    int portIndex = port->itemOfModule();

    return portFinishedStrategy(moduleIndex, portIndex);
}

bool CXenaTester::portTxMode(int moduleIndex, int portIndex, QString &mode)
{
    if (!m_commDev->portTxMode(moduleIndex,portIndex,mode))
    {
        return false;
    }

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setTxMode(mode);

    return true;
}

void CXenaTester::setPortTxModeNormal(int moduleIndex, int portIndex)
{
    m_commDev->setPortTxModeNormal(moduleIndex,portIndex);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setTxMode("Normal");
}

void CXenaTester::setPortTxModeStrict(int moduleIndex, int portIndex)
{
    m_commDev->setPortTxModeStrict(moduleIndex,portIndex);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setTxMode("Strict");
}

void CXenaTester::setPortTxModeSequential(int moduleIndex, int portIndex)
{
    m_commDev->setPortTxModeSequential(moduleIndex,portIndex);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setTxMode("Sequential");
}

bool CXenaTester::portRateFraction(int moduleIndex, int portIndex, float &fraction)
{
    if (!m_commDev->portRateFraction(moduleIndex,portIndex,fraction))
    {
        return false;
    }

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setRateFraction(fraction);

    return true;
}

void CXenaTester::setPortRateFraction(int moduleIndex, int portIndex, float fraction)
{
    m_commDev->setPortRateFraction(moduleIndex,portIndex,fraction);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setRateFraction(fraction);
}

bool CXenaTester::portPacketRate(int moduleIndex, int portIndex, int &packetRate)
{
    if (!m_commDev->portPacketRate(moduleIndex,portIndex,packetRate))
    {
        return false;
    }

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setPacketRate(packetRate);

    return true;
}

void CXenaTester::setPortPacketRate(int moduleIndex, int portIndex, int packetRate)
{
    m_commDev->setPortPacketRate(moduleIndex,portIndex,packetRate);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setPacketRate(packetRate);
}

bool CXenaTester::portBitRate(int moduleIndex, int portIndex, double &bitRate)
{
    if (!m_commDev->portBitRate(moduleIndex,portIndex,bitRate))
    {
        return false;
    }

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setBitRate(bitRate);

    return true;
}

void CXenaTester::setPortBitRate(int moduleIndex, int portIndex, double bitRate)
{
    m_commDev->setPortBitRate(moduleIndex,portIndex,bitRate);

    CPort* port = m_chassis.port(moduleIndex, portIndex);
    port->setBitRate(bitRate);
}

bool CXenaTester::updateStreamHeader(int moduleIndex, int portIndex, int streamIndex,CStream *stream)
{
    bool result = false;
    if (m_commDev->updateStreamHeader(moduleIndex,
                                      portIndex,
                                      streamIndex,
                                      stream))
    {
        result = true;
    }

    return false;
}

bool CXenaTester::streamModifierCount(int moduleIndex, int portIndex, int streamIndex, int &num)
{
    return m_commDev->streamModifierCount(moduleIndex,
                                          portIndex,
                                          streamIndex,
                                          num);
}

bool CXenaTester::setStreamModifierCount(int moduleIndex, int portIndex, int streamIndex,int index)
{
    return m_commDev->setStreamModifierCount(moduleIndex,
                                             portIndex,
                                             streamIndex,
                                             index);
}

void CXenaTester::setStreamModifierInc(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    return m_commDev->setStreamModifierInc(moduleIndex,
                                           portIndex,
                                           streamIndex,
                                           modify);
}

void CXenaTester::setStreamModifierDec(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    return m_commDev->setStreamModifierDec(moduleIndex,
                                           portIndex,
                                           streamIndex,
                                           modify);
}

void CXenaTester::setStreamModifierRam(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    return m_commDev->setStreamModifierRam(moduleIndex,
                                           portIndex,
                                           streamIndex,
                                           modify);
}

void CXenaTester::setStreamModifierRange(int moduleIndex, int portIndex, int streamIndex, S_MODIFY & modify)
{
    return m_commDev->setStreamModifierRange(moduleIndex,
                                             portIndex,
                                             streamIndex,
                                             modify);
}

void CXenaTester::setStreamEnabled(int moduleIndex, int portIndex, int streamIndex, bool enabled)
{
    m_commDev->setStreamEnabled(moduleIndex,
                                portIndex,
                                streamIndex,
                                enabled);

    m_chassis.module(moduleIndex)->port(portIndex)->stream(streamIndex)->enableTx(enabled);
}


/************************************************************************/
/* 配置导入导出接口                                                     */
/************************************************************************/
bool CXenaTester::exportPort(CPort* port, const QString &xmlFileName)
{
    return exportPort(port->module()->itemOfChassis(), port->itemOfModule(), xmlFileName);
}

bool CXenaTester::exportPort(int moduleIndex, int portIndex, const QString &xmlFileName)
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement root = doc.createElement("settings");
    doc.appendChild(root);

    QDomElement type = doc.createElement("template_type");
    type.setAttribute(QString("val"), "port");
    root.appendChild(type);

    QDomElement portEle = doc.createElement(QString("port"));
    if (!CPort::toXml(chassis()->port(moduleIndex, portIndex), doc, portEle)) {
        return false;
    }
    root.appendChild(portEle);

    QFile file(xmlFileName);  
    file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text);
    QByteArray text(doc.toByteArray());
    file.write(text.data(), text.size());
    file.flush();
    file.close();

    return true;
}
bool CXenaTester::exportModule(CModule* module, const QString &xmlFileName)
{
    return exportModule(module->itemOfChassis(), xmlFileName);
}

bool CXenaTester::exportModule(int moduleIndex, const QString &xmlFileName)
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement root = doc.createElement("settings");
    doc.appendChild(root);

    QDomElement type = doc.createElement("template_type");
    type.setAttribute(QString("val"), "module");
    root.appendChild(type);

    QDomElement ele = doc.createElement(QString("module"));
    if (!CModule::toXml(chassis()->module(moduleIndex), doc, ele)) {
        return false;
    }
    root.appendChild(ele);

    QFile file(xmlFileName);  
    file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text);
    QByteArray text(doc.toByteArray());
    file.write(text.data(), text.size());
    file.flush();
    file.close();

    return true;
}

/*
 *  @Param In:
 *      xmlFileName:    xml文件名
 *  @Param In:
 *      moduleIndex:    板卡序号
 *  @Param In:
 *      portIndex:      端口序号
 *  @Param Out:
 *      port:           由配置生产的端口类
 */
bool CXenaTester::importPort(const QString &xmlFileName, int moduleIndex, int portIndex, CPort *port)
{
    if (port == NULL) {
        return false;
    }

    QFile file(xmlFileName);  
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << QString("%1 open %2 fail.").arg(__FUNCTION__).arg(xmlFileName);
        return false;  
    }  
    QDomDocument document;  
    QString error;  
    int row = 0, column = 0;  
    if(!document.setContent(&file, false, &error, &row, &column)) {  
        qDebug() << QString("%1 parse file failed at line row and column %2,%3").arg(__FUNCTION__).arg(row).arg(column);
        return false;  
    }  
    if(document.isNull()) {  
        qDebug() << QString("%1 document is null!").arg(__FUNCTION__);
        return false;  
    }  

    QDomElement root = document.documentElement();  

    QString type = root.firstChildElement(QString("template_type")).attribute(QString("val"));
    if (type != "port") {
        return false;
    }

    QDomElement portEle = root.firstChildElement(QString("port"));
    if (!CPort::fromXml(portEle, *port)) {
        return false;
    }

    return true;
}

/*
 *  @Param In:
 *      xmlFileName:    xml文件名
 *  @Param In:
 *      moduleIndex:    板卡序号
 *  @Param Out:
 *      module:         由配置生产的板卡类
 */
bool CXenaTester::importModule(const QString &xmlFileName, int moduleIndex, CModule *module)
{
    if (module == NULL) {
        return false;
    }

    QFile file(xmlFileName);  
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << QString("%1 open %2 fail.").arg(__FUNCTION__).arg(xmlFileName);
        return false;  
    }  
    QDomDocument document;  
    QString error;  
    int row = 0, column = 0;  
    if(!document.setContent(&file, false, &error, &row, &column)) {  
        qDebug() << QString("%1 parse file failed at line row and column %2,%3").arg(__FUNCTION__).arg(row).arg(column);
        return false;  
    }  
    if(document.isNull()) {  
        qDebug() << QString("%1 document is null!").arg(__FUNCTION__);
        return false;  
    }  

    QDomElement root = document.documentElement();  

    QString type = root.firstChildElement(QString("template_type")).attribute(QString("val"));
    if (type != "module") {
        return false;
    }

    QDomElement portEle = root.firstChildElement(QString("module"));
    if (!CModule::fromXml(portEle, *module)) {
        return false;
    }

    return true;
}