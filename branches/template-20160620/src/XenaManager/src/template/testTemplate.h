/// @file
///     testTemplate.h
///
/// @brief
///     �������ģ�嵼�뵼����
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾
///
/// ���ߣ�
///    yangyuchao  2016.6.23
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
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
    CTestTemplate();                                        //  �����˿ڵ�ģ��
    ~CTestTemplate();

    enum Type {
        TYPE_NONE = -1,
        TYPE_CHASSIS,
        TYPE_MODULE,
        TYPE_PORT
    };

    Type type() {return m_type;}

    //  ��������
    bool saveTemplate(CPort* port, const QString &fileName);
    bool saveTemplate(CModule* module, const QString &fileName);
    bool saveTemplate(CChassis* chassis, const QString &fileName);

    /*
     *  templateType  �鿴ģ���ļ�����������
     */
    Type templateType(const QString &fileName);
    /*
     *  loadTemplate  ��ģ���ļ��е���ʵ��
     *  @Note:
     *      ������ͨ��templateType��ȡģ������
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