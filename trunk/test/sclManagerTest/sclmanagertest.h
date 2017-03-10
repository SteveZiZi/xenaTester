#ifndef SCLMANAGERTEST_H
#define SCLMANAGERTEST_H

#include <QtGui/QDialog>
#include "XenaManager\src\dialog\ImportData\ImportDataDialog.h"

class QStandardItem;
class QTreeView;
class sclManagerTest : public QDialog
{
    Q_OBJECT

public:
    sclManagerTest(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~sclManagerTest();

private slots:
    void importScdFile();
    void importPcapFile();

    void login();
    void testTemplate();
    void modifierTest();
    void showFrame();

    void frameModified(QStandardItem *item);

private:
    void _importFile(IMPORT_FILE_STYLE fileStyle, const QString& fileName);

private:

    ImportData m_importData;
};

#endif // SCLMANAGERTEST_H
