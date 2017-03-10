#pragma once

#include <QtGui/QLineEdit>

class CEthSegEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit CEthSegEdit(QWidget* parent = 0, bool trrigleEdit = true);
    explicit CEthSegEdit(const QString &text, QWidget* parent=0, bool trrigleEdit = true);
    virtual ~CEthSegEdit();


protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void focusInEvent(QFocusEvent *);

private:
    void startEdit(void);
    void endEdit(void);

private:
    bool m_isEditMode;
};
