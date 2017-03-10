#include "sclmanagertest.h"
#include <QtGui/QApplication>
#include <QTextCodec>


#if (defined _DEBUG) && (defined WIN32)
#include "vld.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));


    sclManagerTest w;
    w.show();
    return a.exec();
}
