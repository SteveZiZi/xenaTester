#include "xenamanager.h"
#include <QTextCodec>
#include <QFont>
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	 
	// 支持中文
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

    XenaManager w;
    w.show();
    return a.exec();
}
