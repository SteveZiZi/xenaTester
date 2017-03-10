#include "ethSegEdit.h"

#include <QtGui/QKeyEvent>

CEthSegEdit::CEthSegEdit(const QString& text, QWidget* parent/* =0 */, bool trrigleEdit/* = true*/)
: QLineEdit(text, parent)
, m_isEditMode(false)
{
}

CEthSegEdit::CEthSegEdit(QWidget* parent/* =0 */, bool trrigleEdit/* = true*/)
: QLineEdit(parent)
, m_isEditMode(false)
{
}

CEthSegEdit::~CEthSegEdit()
{

}

void CEthSegEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    setSelection(0, 2);
}


void CEthSegEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    switch(event->key()) {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        event->accept();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Escape:
        this->setHidden(true);  //¶ªÊ§½¹µãÈÃ¸¸´°¿ÚÏìÓ¦
        break;
    default:
        QLineEdit::keyPressEvent(event);
    }
}


void CEthSegEdit::startEdit(void)
{
    m_isEditMode = true;
}

void CEthSegEdit::endEdit(void)
{
    m_isEditMode = false;
}