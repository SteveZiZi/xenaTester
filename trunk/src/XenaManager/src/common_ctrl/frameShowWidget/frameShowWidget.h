#pragma once

#include <QTextEdit>
#include <QList>

class CFrameShowWidget : public QTextEdit
{
public:
    struct TextPos{
        quint32 pos;            //  文本的位置
        quint32 length;         //  文本的长度
        TextPos() : pos (-1), length(0) {}
        TextPos(quint32 pos, quint32 lenght) {this->pos = pos; this->length = lenght;}
        bool operator == (const TextPos & pos) {
            return (this->pos == pos.pos && this->length == pos.length);
        }
    };

    CFrameShowWidget(QWidget *parent = 0);
    CFrameShowWidget(const quint8 *frame, quint32 length, QWidget *parent = 0);

    void setFrame(const quint8 *frame, quint32 length);

    bool isSelected(quint32 pos);
    bool isHighLihgt(quint32 pos);

public slots:

    /*
     *  @Param:
     *      pos:        数据的位置
     *      length:     数据的长度
     */
    bool appendHighLight(quint32 pos, quint32 length);
    /*
     *  @Param:
     *      pos:        数据的位置
     *      length:     数据的长度
     */
    bool removeHighLight(quint32 pos, quint32 length);
    /*
     *  @Param:
     *      pos:        数据的位置
     *      length:     数据的长度
     */
    bool setSelected(quint32 pos, quint32 length);
    void clearSelected();

    //只能更新已有报文的内容，不能追加长度
    void updateFrame(quint32 pos, quint32 length, const quint8 *frame);


private:
    void _init();
    void frameToString(const quint8 *frame, quint32 length, QString &text_);

    quint32 textPos(quint32 framePos_);

    void setTextFormat(quint32 pos, quint32 length, const QTextCharFormat &format);
    void setDefaultFormat();

    void updateText(quint32 pos, const QString& text_);

    void mergeTextPos(QList<TextPos> &list);
private:
    QList<TextPos> m_highLightPos;

    QString m_text;

    QTextCharFormat m_defaultFormat;
    QTextCharFormat m_hightLightFormat;
    QTextCharFormat m_selectedFormat;

    bool            m_hasSelectedText;
    TextPos         m_selectedPos;
};