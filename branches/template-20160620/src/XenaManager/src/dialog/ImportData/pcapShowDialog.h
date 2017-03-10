#pragma once
#include "ImportDataDialog.h"

class QTableWidget;
class CPcapAnalysis;
class QLabel;
class QAction;
class CPcapShowDialog : public ImportDataDialog
{
    Q_OBJECT
public:
    enum TABLE_COLUMN {
        COL_ITEM,
        COL_TYPE,
        COL_APPID,
        COL_MAC_ADDR,
        COL_LENGTH,

        MAX_COL_NUM
    };

    CPcapShowDialog(QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~CPcapShowDialog();

    virtual bool parseFile(const QString& scdFile);
    virtual bool selectedFrame(ImportData &data);

private slots:
    void exprotCB();
    void onCustomContextMenuRequested(const QPoint &point);

private:
    void _setupUI();
    void _layout();
    QTableWidget* createTableWidget();

    void cleaerResource();
private:
    CPcapAnalysis *m_analysis;

    QLabel* m_labelFileName;
    QTableWidget* m_tableWidget;
    QString m_pcapFileName;
    QAction*  m_selectionAction;

    int m_selectedItem;
};