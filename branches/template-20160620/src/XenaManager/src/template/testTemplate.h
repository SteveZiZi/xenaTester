/// @file
///     testTemplate.h
///
/// @brief
///     定义测试模板导入导出类
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司
///
/// 作者：
///    yangyuchao  2016.6.23
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///

#pragma once

#include <QString>

class CChassis;
class CModule;
class CPort;
class CStream;
class QDomDocument;
class QDomElement;
class CTestTemplate
{
public:
    CTestTemplate();                                        //  导出端口的模板
    ~CTestTemplate();

    enum Type {
        TYPE_NONE = -1,
        TYPE_CHASSIS,
        TYPE_MODULE,
        TYPE_PORT
    };

    Type type() {return m_type;}

    //  导出功能
    bool saveTemplate(CPort* port, const QString &fileName);
    bool saveTemplate(CModule* module, const QString &fileName);
    bool saveTemplate(CChassis* chassis, const QString &fileName);

    /*
     *  templateType  查看模板文件的数据类型
     */
    Type templateType(const QString &fileName);
    /*
     *  loadTemplate  从模板文件中导出实例
     *  @Note:
     *      必须先通过templateType获取模板类型
     */
    bool loadTemplate(const QString &fileName, CChassis *chassis);
    bool loadTemplate(const QString &fileName, CModule *module);
    bool loadTemplate(const QString &fileName, CPort *port);

private:
    bool exportStream(QDomDocument &doc, QDomElement &parent, CStream* stream);
    bool importStream(QDomElement &streamElem, CStream* stream);
    bool exportPort(QDomDocument &doc, QDomElement &parent, CPort* port);
    bool importPort(QDomElement &portElem, CPort* port);
    bool exportModule(QDomDocument &doc, QDomElement &parent, CModule* module);
    bool importModule(QDomElement &moduleElem, CModule* module);

    bool saveXml(const QString &fileName, const QDomDocument &doc);
    bool loadXml(const QString &fileName, QDomDocument &doc, QDomElement &root);


private:
    Type m_type;
};