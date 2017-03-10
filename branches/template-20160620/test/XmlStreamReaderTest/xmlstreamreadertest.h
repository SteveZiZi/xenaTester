#ifndef XMLSTREAMREADERTEST_H
#define XMLSTREAMREADERTEST_H

#include <QtGui/QDialog>
#include "ui_xmlstreamreadertest.h"

class XmlStreamReaderTest : public QDialog
{
    Q_OBJECT

public:
    XmlStreamReaderTest(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~XmlStreamReaderTest();

private:
    Ui::XmlStreamReaderTestClass ui;
};

#endif // XMLSTREAMREADERTEST_H
