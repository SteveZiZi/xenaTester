#pragma once

#include "ImportDataDialog.h"
#include <QVector>

#include "sclManager/include/sclManager.h"
#include "scdCommon.h"

typedef QVector<IED_INFO*> VECTOR_IED_INFO;

class QTreeWidget;
class CCBTabWidget;
class QLabel;
class QTreeWidgetItem;
class CIedShowWidget : public ImportDataDialog
{
    Q_OBJECT
public:
    CIedShowWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~CIedShowWidget();

    virtual bool parseFile(const QString& scdFile);
    virtual bool selectedFrame(ImportData &data);

    int GetIedNum() {return m_vectorIEDInfo.size();}
    int GetOneIedInfo(int item, IED_INFO & iedInfo);

private slots:
    void updateIedInfo(const IED_INFO* iedInfo);
    void cbSelect(const SelectedCBInfo &cbInfo, const quint8* data, quint32 size);

    void iedTreeChanged();

private:
    bool addIed(const SCDSMV92ANALYSIS_IEDSMV_INFO* iedInfo);
    bool addIed(const SCDGOOSEANALYSIS_IEDGOOSE_INFO* iedInfo);
    /*
     *  @Func:  
     *  
     *  @Return:
     *      -1:     not found
     *     >=0:     IED position;
     */
    int findIedByName(const QString & name);

    void _setupUI();
    void _layout();
    QTreeWidget* createIEDTree();
    CCBTabWidget* createCBTabWidget();

    void cleanResource();

private:
    QLabel* m_iedLabel;
    QTreeWidget* m_treeWidgetIED;
    CCBTabWidget* m_cbShowWidget;

    QString m_scdFileName;
    CSCLManager* m_sclMgr;
    VECTOR_IED_INFO m_vectorIEDInfo;

    IED_INFO* m_curIed;

    quint8* m_cbFrame;
    quint32 m_length;
    QString m_frameType;
    quint16 m_appid;

};