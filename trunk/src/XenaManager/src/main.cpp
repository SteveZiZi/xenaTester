#include "xenamanager.h"
#include <QTextCodec>
#include <QFont>
#include <QTranslator>
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(xenamanager);
	 
	// 支持中文
// 	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
// 	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
// 	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));


    //加载语言包
    QTranslator translator;
    bool b = false;
    b = translator.load("xenamanager_zh.qm");
    a.installTranslator(&translator);

    XenaManager w;
    w.show();
    return a.exec();
}
