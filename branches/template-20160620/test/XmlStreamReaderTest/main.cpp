#include "xmlstreamreadertest.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XmlStreamReaderTest w;
    w.show();
    return a.exec();
}
