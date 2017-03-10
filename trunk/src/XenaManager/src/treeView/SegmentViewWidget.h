/*  @file
 *  @brief �����ݳ�����
 *  @author MSZ
 *  @date 2016/05/13
 *  @version 0.1
 */
#ifndef SEGMENTVIEWWIDGET_H
#define SEGMENTVIEWWIDGET_H

#include <QWidget>
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"
#include "XenaManager/src/dialog/ImportData/utilTools.h"
#include "XenaManager/src/dialog/modifierDialog/modifierDialog.h"

class CXenaTester;
class CStream;
class QTreeView;
class QStandardItemModel;
class QAction;
class StreamConfigWidget;
class CFrameShowWidget;
class SegmentViewWidget : public QWidget
{
	Q_OBJECT

public:
    explicit SegmentViewWidget(CXenaTester *xenaTester,QWidget *parent = 0);
    ~SegmentViewWidget();

signals:
    void updateStreamLength(quint16 length);							// �����������ý����еı��ĳ���

public slots:
    void updateSegment();                                               // ���stream����ͬ��������������

    void onAddModifierAction();                                         // 
    void onDelModifierAction();
    void onEditModifierAction();
    void onCustomContextMenuRequested(const QPoint &point);             // �Ҽ��˵�����

    void onItemChanged(QStandardItem * item);                           // ���ݷ����仯ʱ�����ݽ����޸�

private slots:
    void itemSelected(const QModelIndex &index);

private:
    void initUI();
    void setMainLayout();
    void setSegmentData();
    void updateView();													// ����segmentView�е�����
    void updateCurrentIndex();
    void updateStream();
    void updateModifier();

    void createActions();
    void createContextMenu();                                           // �����Ҽ��˵�
    void setModifierCommand(CModifierDialog::ModifierData *modifierData,int modifierIndex); // ͨ��ָ������������
    void addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex,QModelIndex & modelIndex);
    void addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex);
    void addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex, QStandardItem *item);
    void removeModifier();
    QString createModifierItemData(CModifierDialog::ModifierData modifierData); // ����modifier����
    CModifierDialog::ModifierData parseModifierItemData(const QString & itemStr);

private:
    ImportData              m_importData;
    QModelIndex             m_currentIndex;
    CStream                 *m_pStream;

    CXenaTester             *m_pXenaTester;
    StreamConfigWidget      *m_pStreamWidget;

    QTreeView               *m_pSegmentTreeView;
    QStandardItemModel      *m_pSegmentModel;

    CFrameShowWidget        *m_pFrameShowWidget;

    QAction                 *m_pAddModifierAction;
    QAction                 *m_pDelModifierAction;
    QAction                 *m_pEditModifierAction;

    int                     m_currentModuleIndex;
    int                     m_currentPortIndex;
    int                     m_currentStreamIndex;
};

#endif // SEGMENTVIEWWIDGET_H
