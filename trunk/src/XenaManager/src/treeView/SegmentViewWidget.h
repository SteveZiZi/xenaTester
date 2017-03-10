/*  @file
 *  @brief 流数据呈现类
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
    void updateStreamLength(quint16 length);							// 更新流的配置界面中的报文长度

public slots:
    void updateSegment();                                               // 点击stream流，同步更新数据内容

    void onAddModifierAction();                                         // 
    void onDelModifierAction();
    void onEditModifierAction();
    void onCustomContextMenuRequested(const QPoint &point);             // 右键菜单功能

    void onItemChanged(QStandardItem * item);                           // 数据发生变化时对数据进行修改

private slots:
    void itemSelected(const QModelIndex &index);

private:
    void initUI();
    void setMainLayout();
    void setSegmentData();
    void updateView();													// 更新segmentView中的内容
    void updateCurrentIndex();
    void updateStream();
    void updateModifier();

    void createActions();
    void createContextMenu();                                           // 创建右键菜单
    void setModifierCommand(CModifierDialog::ModifierData *modifierData,int modifierIndex); // 通过指令设置跳变域
    void addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex,QModelIndex & modelIndex);
    void addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex);
    void addModifier(CModifierDialog::ModifierData *modifierData,int modifierIndex, QStandardItem *item);
    void removeModifier();
    QString createModifierItemData(CModifierDialog::ModifierData modifierData); // 创建modifier内容
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
