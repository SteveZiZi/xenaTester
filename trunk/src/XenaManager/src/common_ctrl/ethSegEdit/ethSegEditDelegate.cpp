#include "ethSegEditDelegate.h"

#include "ethSegEdit.h"

CEthSegEditDelegate::CEthSegEditDelegate(QObject *parent)
: QItemDelegate(parent)
{
}

QWidget *CEthSegEditDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex & index ) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    int dataNum = value.length();
    if (dataNum%3 != 0) {
        dataNum = dataNum / 3 + 1;
    }
    else {
        dataNum /= 3;
    }

    QString mask(">");
    for(int i = 0; i < dataNum; i++) {
        mask += "HH";
        if (i+1 != dataNum)
            mask += ' ';
    }
    CEthSegEdit *editor = new CEthSegEdit(parent);    
    editor->setInputMask(mask);
    editor->setValidator(new QRegExpValidator(QRegExp("[A-F][0-9]"), parent));
    return editor;
}

void CEthSegEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
}

void CEthSegEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    model->setData(index, lineEdit->text());
}

void CEthSegEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
