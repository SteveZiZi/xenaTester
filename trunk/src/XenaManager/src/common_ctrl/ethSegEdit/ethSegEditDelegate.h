#pragma once

#include <QtGui/QItemDelegate>

class CEthSegEditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    CEthSegEditDelegate(QObject *parent = 0);

    //override from QItemDelegate
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
