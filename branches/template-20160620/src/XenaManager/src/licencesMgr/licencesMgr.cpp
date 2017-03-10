#include "licencesMgr.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>

#include <QDebug>

static bool loadXml(const QString &fileName, QDomDocument &doc, QDomElement &root)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QString string(QString("loadXml open %1 fail.").arg(fileName));
        qDebug() << string;
        return false;  
    }  

    QString error;  
    int row = 0, column = 0;  
    if(!doc.setContent(&file, false, &error, &row, &column)) {  
        qDebug() << QString("loadXml parse file failed at line row and column %1,%2").arg(row).arg(column);
        return false;  
    }  
    file.close();

    if(doc.isNull()) {  
        qDebug() << "loadTemplate document is null!";
        return false;  
    }  

    root = doc.documentElement();  
    if (root.isNull()) {
        return false;
    }

    return true;
}
/*
 *  @Func:  licencesLib_isActive
 *  @Desc:  �����ṩ��SN���ж�licences�Ƿ���Ч
 *  
 *  @Param[in]:
 *      name:       ��Ʒ��
 *      arrSN:      ���SN�ŵ�ָ��
 *      length:     SN�ŵĳ���
 *      licences:   ���licences��ָ��
 *      size:       licences�Ĵ�С
 *  
 *  @Return:
 *      true:       SN�Ŷ�Ӧ��licences��Ч
 *      false:      SN�Ŷ�Ӧ��licences��Ч
 */
__declspec(dllimport) bool licencesLib_isActive(const std::string &name, const char *arrSN, int length, const char *licences, int size);


bool CLicencesMgr::isActive(const QString &licencesFile)
{
    QDomDocument document;  
    QDomElement root;

    if (loadXml(licencesFile, document, root)) {

        QString product_name = root.firstChildElement("product").attribute("val");
        QString sn = root.firstChildElement("SN").attribute("val");
        QString licences = root.firstChildElement("CODE").attribute("val");

        return licencesLib_isActive(product_name.toStdString(), 
                                    sn.toStdString().c_str(), 
                                    sn.length(),
                                    licences.toStdString().c_str(), 
                                    licences.length());
    }

    return false;
}