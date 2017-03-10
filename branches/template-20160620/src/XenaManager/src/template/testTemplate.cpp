#include "testTemplate.h"

#include "XenaManager/src/Communicate/chassis.h"
#include "XenaManager/src/Communicate/module.h"
#include "XenaManager/src/Communicate/port.h"
#include "XenaManager/src/Communicate/stream.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>

#include <QDebug>

CTestTemplate::CTestTemplate()
: m_type(TYPE_NONE)
{

}

CTestTemplate::~CTestTemplate()
{

}


bool CTestTemplate::saveTemplate(CPort* port, const QString &fileName)
{
    QDomDocument doc;  
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement root = doc.createElement("config");
    doc.appendChild(root);

    exportPort(doc, root, port);

    return saveXml(fileName, doc);
}

bool CTestTemplate::saveTemplate(CModule* module, const QString &fileName)
{
    QDomDocument doc;  
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement root = doc.createElement("config");
    doc.appendChild(root);

    exportModule(doc, root, module);

    return saveXml(fileName, doc);
}

bool CTestTemplate::saveTemplate(CChassis* chassis, const QString &fileName)
{
    return false;
}



CTestTemplate::Type CTestTemplate::templateType(const QString &fileName)
{
    QDomDocument document;  
    QDomElement root;
    m_type = TYPE_NONE;
    if (loadXml(fileName, document, root)) {
        if (!root.firstChildElement("chassis").isNull()) {
            m_type = TYPE_CHASSIS;
        }
        else if (!root.firstChildElement("module").isNull()) {
            m_type = TYPE_MODULE;
        }
        else if (!root.firstChildElement("port").isNull()) {
            m_type = TYPE_PORT;
        }
    }

    return m_type;
}


bool CTestTemplate::loadTemplate(const QString &fileName, CChassis *chassis)
{
    Q_ASSERT(false);

    return false;
}

bool CTestTemplate::loadTemplate(const QString &fileName, CModule *module)
{
    if (m_type != TYPE_MODULE) {
        return false;
    }

    QDomDocument document;  
    QDomElement root;

    if (loadXml(fileName, document, root)) {
        QDomElement moduleElem = root.firstChildElement("module");
        if (!moduleElem.isNull() && importModule(moduleElem, module)) {
            return true;
        }
    }

    return false;
}

bool CTestTemplate::loadTemplate(const QString &fileName, CPort *port)
{
    if (m_type != TYPE_PORT) {
        return false;
    }

    QDomDocument document;  
    QDomElement root;
    if (loadXml(fileName, document, root)) {
        QDomElement moduleElem = root.firstChildElement("port");
        if (!moduleElem.isNull() && importPort(moduleElem, port)) {
            return true;
        }
    }

    return false;
}



bool CTestTemplate::exportStream(QDomDocument &doc, QDomElement &parent, CStream* stream)
{
    QDomElement streamElem = doc.createElement("stream");

    //  comment
    QDomElement elem = doc.createElement("name");
    elem.setAttribute(QString("val"), stream->streamName());
    streamElem.appendChild(elem);

    //  enable
    elem = doc.createElement("enable");
    elem.setAttribute(QString("val"), stream->isEnableTx());
    streamElem.appendChild(elem);

    //  settingsType
    elem = doc.createElement("settingsType");
    elem.setAttribute(QString("val"), stream->settingsType());
    streamElem.appendChild(elem);

    //  tx percent
    if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
        elem = doc.createElement("txPercent");
        elem.setAttribute(QString("val"), stream->txPercent());
        streamElem.appendChild(elem);
    }
    else {
        elem = doc.createElement("framesPerSecond");
        elem.setAttribute(QString("val"), stream->txFramesPerSecond());
        streamElem.appendChild(elem);
    }

    //  strategy
    elem = doc.createElement("strategy");
    {
        //  type
        QDomElement child = doc.createElement("type");
        child.setAttribute(QString("val"), stream->strategy());
        elem.appendChild(child);

        // packetLimit
        child = doc.createElement("packetLimit");
        child.setAttribute(QString("val"), stream->packetLimit());
        elem.appendChild(child);

        if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
            // percent
            child = doc.createElement("percent");
            child.setAttribute(QString("start"), stream->startPercent());
            child.setAttribute(QString("end"), stream->endPercent());
            child.setAttribute(QString("step"), stream->stepPercent());
            elem.appendChild(child);
        }
        else {
            // percent
            child = doc.createElement("framesPerSecond");
            child.setAttribute(QString("start"), stream->startFramesPerSec());
            child.setAttribute(QString("end"), stream->endFramesPerSec());
            child.setAttribute(QString("step"), stream->stepFramesPerSec());
            elem.appendChild(child);
        }

        // loop cnt
        child = doc.createElement("loopCnt");
        child.setAttribute(QString("val"), stream->loopCnt());
        elem.appendChild(child);
    }
    streamElem.appendChild(elem);

    // frame data
    elem = doc.createElement("frameData");
    {
        const ImportData* data = stream->const_data();
        //  style
        QDomElement child = doc.createElement("frameStyle");
        child.setAttribute(QString("val"), data->frameStyle);
        elem.appendChild(child);

        // appid
        child = doc.createElement("appId");
        child.setAttribute(QString("val"), QString::number(data->appId, 16));
        elem.appendChild(child);

        // iedName
        child = doc.createElement("iedName");
        child.setAttribute(QString("val"), data->iedName);
        elem.appendChild(child);

        // frameLength
        int length = data->frameLength;
        child = doc.createElement("frameLength");
        child.setAttribute(QString("val"), length);
        elem.appendChild(child);

        if (length) {
            //  frame
            QString text;
            text.reserve(length*3);
            const quint8* data = stream->frames();
            for(int i = 0; i < length; i++) {
                text += QString::number(data[i], 16);
                if (i+1 != length) {
                    text.append(' ');
                }
            }
            child = doc.createElement("frame");
            child.appendChild(doc.createTextNode(text));
            elem.appendChild(child);
        }
    }
    streamElem.appendChild(elem);

    parent.appendChild(streamElem);

    return true;
}

bool CTestTemplate::importStream(QDomElement &streamElem, CStream* stream)
{
    bool rc = false;
    // comment
    QDomElement elem = streamElem.firstChildElement("name");
    if (!elem.isNull())
        stream->setStreamName(elem.attribute("val"));
    else goto exitCode;

    //  enable
    elem = streamElem.firstChildElement("enable");
    if (!elem.isNull())
        stream->enableTx(static_cast<bool>(elem.attribute("val").toInt()));
    else goto exitCode;

    //  settingsType
    elem = streamElem.firstChildElement("settingsType");
    if (!elem.isNull()) {
        if (elem.attribute("val").toInt() == CStream::SETTINGS_PERCENT)
            stream->setSettingsType(CStream::SETTINGS_PERCENT);
        else
            stream->setSettingsType(CStream::SETTINGS_PPS);
    }
    else goto exitCode;

    if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
        elem = streamElem.firstChildElement("txPercent");
        if (!elem.isNull())
            stream->setTxPercent(elem.attribute("val").toFloat());
        else goto exitCode;
    }
    else {
        elem = streamElem.firstChildElement("framesPerSecond");
        if (!elem.isNull())
            stream->setTxFramesPerSecond(elem.attribute("val").toInt());
        else goto exitCode;
    }

    //  strategy
    elem = streamElem.firstChildElement("strategy");
    if (!elem.isNull())
    {
        int strategy;
        QDomElement child = elem.firstChildElement("type");
        if (!child.isNull()) {
            strategy = elem.attribute("val").toInt();
        }
        else goto exitCode;

        if (strategy == CStream::STRATEGY_NONE) {
            stream->setStrategy();
        }
        else {
            int framesNum = 0;
            child = elem.firstChildElement("packetLimit");
            if (!child.isNull()) {
                framesNum = elem.attribute("val").toInt();
            }
            else goto exitCode;

            if (strategy == CStream::STRATEGY_FRAMES) {
                stream->setStrategy(framesNum);
            }
            else {
                int loopCnt = 0;
                child = elem.firstChildElement("loopCnt");
                if (!child.isNull()) {
                    loopCnt = child.attribute("val").toInt();
                }
                else goto exitCode;

                if (stream->settingsType() == CStream::SETTINGS_PERCENT) {
                    float start, end, step;
                    child = elem.firstChildElement("percent");
                    if (!child.isNull()) {
                        start = elem.attribute("start").toFloat();
                        end = elem.attribute("end").toFloat();
                        step = elem.attribute("step").toFloat();
                    }
                    else goto exitCode;
                    stream->setStrategy(framesNum, start, end, step, loopCnt);
                }
                else {
                    int start, end, step;
                    child = elem.firstChildElement("framesPerSecond");
                    if (!child.isNull()) {
                        start = elem.attribute("start").toInt();
                        end = elem.attribute("end").toInt();
                        step = elem.attribute("step").toInt();
                    }
                    else goto exitCode;
                    stream->setStrategy(framesNum, start, end, step, loopCnt);
                }
            }
        }
    }
    else goto exitCode;

    //  frame data
    elem = streamElem.firstChildElement("frameData");
    if (!elem.isNull())
    {
        ImportData* data = stream->data();
        //  style
        QDomElement child = elem.firstChildElement("frameStyle");
        if (!child.isNull()) {
            data->frameStyle = elem.attribute("val");
        }
        else goto exitCode;

        // appid
        child = elem.firstChildElement("appId");
        if (!child.isNull()) {
            data->appId = elem.attribute("val").toInt(0, 16);
        }
        else goto exitCode;

        //  iedName
        child = elem.firstChildElement("iedName");
        if (!child.isNull()) {
            data->iedName = elem.attribute("val");
        }
        else goto exitCode;

        // frameLength
        child = elem.firstChildElement("frameLength");
        if (!child.isNull()) {
            data->frameLength = elem.attribute("val").toInt();
        }
        else goto exitCode;

        //  frame
        if (data->frameLength) {
            child = elem.firstChildElement("frame");
            QDomText domText = child.firstChild().toText();
            if (!domText.isNull()) {
                QStringList splitText = domText.data().split(' ');
                int pos = 0;
                Q_FOREACH(const QString &str, splitText) {
                    if (pos < data->frameLength)
                        data->frame[pos] = str.toUInt(0, 16);
                    else
                        break;
                }
            }
            else goto exitCode;
        }
    }
    else goto exitCode;

    rc = true;
exitCode:
    return rc;
}

bool CTestTemplate::exportPort(QDomDocument &doc, QDomElement &parent, CPort* port)
{
    QDomElement portElem = doc.createElement("port");

    //  name
    QDomElement elem = doc.createElement("name");
    elem.setAttribute(QString("val"), port->portName());
    portElem.appendChild(elem);

    //  max header length
    elem = doc.createElement("maxHeaderLength");
    elem.setAttribute(QString("val"), port->maxHeaderLength());
    portElem.appendChild(elem);

    //  tx limit time
    elem = doc.createElement("txTimeLimit");
    elem.setAttribute(QString("val"), port->txTimeLimit());
    portElem.appendChild(elem);

    //  real speed
    elem = doc.createElement("realSpeed");
    elem.setAttribute(QString("val"), port->realSpeed());
    portElem.appendChild(elem);

    //  status
    elem = doc.createElement("status");
    elem.setAttribute(QString("linked"), port->linked());
    elem.setAttribute(QString("actived"), port->actived());
    portElem.appendChild(elem);

    //  streams
    elem = doc.createElement("streams");
    elem.setAttribute(QString("num"), port->streamCount());
    for(int i = 0; i < port->streamCount(); i++) {
        exportStream(doc, elem, port->stream(i));
    }
    portElem.appendChild(elem);

    parent.appendChild(portElem);

    return true;
}

bool CTestTemplate::importPort(QDomElement &portElem, CPort* port)
{
    bool rc = false;

    //name 
    QDomElement elem = portElem.firstChildElement("name");
    if (!elem.isNull())
        port->setPortName(elem.attribute("val"));
    else goto exitCode;

    //  max header length
    elem = portElem.firstChildElement("maxHeaderLength");
    if (!elem.isNull())
        port->setMaxHeaderLength(elem.attribute("val").toInt());
    else goto exitCode;

    //  tx limit time
    elem = portElem.firstChildElement("txTimeLimit");
    if (!elem.isNull())
        port->setTxTimeLimit(elem.attribute("val").toInt());
    else goto exitCode;

    //  real speed
    elem = portElem.firstChildElement("realSpeed");
    if (!elem.isNull())
        port->setRealSpeed(elem.attribute("val").toInt());
    else goto exitCode;

    //  status
    elem = portElem.firstChildElement("status");
    if (!elem.isNull()) {
        port->setLinked(static_cast<bool>(elem.attribute("linked").toInt()));
        port->setActived(static_cast<bool>(elem.attribute("actived").toInt()));
    }
    else goto exitCode;

    //  streams
    elem = portElem.firstChildElement("streams");
    if (!elem.isNull()) {
        int num = elem.attribute("num").toInt();
        for(int i = 0; i < num; i++) {
            QDomElement child;
            if (i == 0) child = elem.firstChildElement("stream");
            else child = elem.nextSiblingElement("stream");
            if (!child.isNull()) {
                CStream* stream = new CStream(port);
                if (importStream(child, stream)) {
                    port->appendStream(stream);
                }
                else {
                    delete stream;
                    goto exitCode;
                }
            }
            else goto exitCode;
        }
    }
    else goto exitCode;

    rc = true;
exitCode:
    return rc;
}


bool CTestTemplate::exportModule(QDomDocument &doc, QDomElement &parent, CModule* module)
{
    QDomElement moduleElem = doc.createElement("module");

    //  module name
    QDomElement elem = doc.createElement("name");
    elem.setAttribute(QString("val"), module->name());
    moduleElem.appendChild(elem);

    //  ports
    elem = doc.createElement("ports");
    elem.setAttribute(QString("num"), module->portCount());
    for(int i = 0; i < module->portCount(); i++) {
        exportPort(doc, elem, module->port(i));
    }
    moduleElem.appendChild(elem);

    parent.appendChild(moduleElem);

    return true;
}


bool CTestTemplate::importModule(QDomElement &moduleElem, CModule* module)
{
    bool rc = false;

    //name 
    QDomElement elem = moduleElem.firstChildElement("name");
    if (!elem.isNull())
        module->setName(elem.attribute("val"));
    else goto exitCode;

    //  ports
    elem = moduleElem.firstChildElement("ports");
    if (!elem.isNull()) {
        int num = elem.attribute("num").toInt();
        for(int i = 0; i < num; i++) {
            QDomElement child;
            if (i == 0) child = elem.firstChildElement("port");
            else child = elem.nextSiblingElement("port");
            if (!child.isNull()) {
                CPort* port = new CPort();
                if (importPort(child, port)) {
                    module->appendPort(port);
                }
                else {
                    delete port;
                    goto exitCode;
                }
            }
            else goto exitCode;
        }
    }
    else goto exitCode;

    rc = true;
exitCode:
    return rc;
}

bool CTestTemplate::saveXml(const QString &fileName, const QDomDocument &doc)
{
    if(fileName.isEmpty())
        return false;  

    QFile file(fileName);  
    if(!file.open(QFile::ReadWrite | QFile::Truncate | QFile::Text)) {
        QString string(QString("CTestTemplate::saveXml open %1 fail.").arg(fileName));
        qDebug() << string;
        return false;  
    }  

    QByteArray text(doc.toByteArray());
    file.write(text.data(), text.size());
    file.flush();
    file.close();

    return true;
}

bool CTestTemplate::loadXml(const QString &fileName, QDomDocument &doc, QDomElement &root)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QString string(QString("CTestTemplate::loadXml open %1 fail.").arg(fileName));
        qDebug() << string;
        return false;  
    }  

    QString error;  
    int row = 0, column = 0;  
    if(!doc.setContent(&file, false, &error, &row, &column)) {  
        qDebug() << QString("CTestTemplate::loadTemplate parse file failed at line row and column %1,%2").arg(row).arg(column);
        return false;  
    }  
    file.close();

    if(doc.isNull()) {  
        qDebug() << "CTestTemplate::loadTemplate document is null!";
        return false;  
    }  

    root = doc.documentElement();  
    if (root.isNull()) {
        return false;
    }

    return true;
}