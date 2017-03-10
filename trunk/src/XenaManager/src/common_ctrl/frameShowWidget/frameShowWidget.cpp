#include "frameShowWidget.h"

#include <QDebug>

static const QString s_headerItem  = "0  1  2  3  4  5  6  7   8  9  10 11 12 13 14 15";
static const QString s_headerSplit = "== == == == == == == ==  == == == == == == == ==";

#define HEADER_LENGTH       98      //报文头的长度

#define DATA_LENGTH         3
#define ROW_FRAME_LENGTH    16                          //一行显示的报文长度
#define ROW_LENGTH          49                          //一行的字符个数  包括\n
#define BLOCK_ROWS          4                           //一块显示多少行
#define BLOCK_FRAME_LENGTH (BLOCK_ROWS*ROW_FRAME_LENGTH)
#define BLOCK_LENGTH       (ROW_LENGTH*BLOCK_ROWS+1)    //一块的字符个数  包括\n

#define FONT_WEIGHT         11



static bool textPosLessThan(const CFrameShowWidget::TextPos &pos1, const CFrameShowWidget::TextPos &pos2)
{
    return pos1.pos < pos2.pos;
}


CFrameShowWidget::CFrameShowWidget(QWidget *parent/* = 0*/)
: QTextEdit(parent)
, m_text("")
, m_hasSelectedText(false)
{
    frameToString(0, 0, m_text);
    _init();
}

CFrameShowWidget::CFrameShowWidget(const quint8 *frame, quint32 length, QWidget *parent/* = 0*/)
: QTextEdit(parent)
, m_hasSelectedText(false)
{
    frameToString(frame, length, m_text);
    _init();
}

void CFrameShowWidget::setFrame(const quint8 *frame, quint32 length)
{
    Q_FOREACH(const TextPos &highLightPos, m_highLightPos)
    {
        removeHighLight(highLightPos.pos, highLightPos.length);
    }
    m_highLightPos.clear();

    frameToString(frame, length, m_text);
    this->setText(m_text);

    setDefaultFormat();
}

bool CFrameShowWidget::isSelected(quint32 pos)
{
    if(m_hasSelectedText) {
        return (pos >= m_selectedPos.pos  &&  pos < m_selectedPos.pos+m_selectedPos.length);
    }
    return false;
}

bool CFrameShowWidget::isHighLihgt(quint32 pos)
{
    Q_FOREACH(const TextPos &highLightPos, m_highLightPos)
    {
        if (pos < highLightPos.pos) {
            return false;
        }
        if (pos < highLightPos.pos+highLightPos.length) {
            return true;
        }
    }

    return false;
}

/*
 *  @Param:
 *      pos:        数据的位置
 *      length:     数据的长度
 */
bool CFrameShowWidget::appendHighLight(quint32 pos, quint32 length)
{
    quint32 textPos_ = textPos(pos);
    quint32 textLength = 2;
    if (length > 1) {
        textLength += textPos(pos+length-1) - textPos_;
    }
    if (textPos_ >= m_text.size() || textPos_+textLength > m_text.size()) {
        return false;
    }

    m_highLightPos.push_back(TextPos(textPos_, textLength));
    qSort(m_highLightPos.begin(), m_highLightPos.end(), textPosLessThan);

    QTextCharFormat mergeFormat = m_hightLightFormat;
    mergeFormat.merge(m_selectedFormat);
    bool selected = isSelected(textPos_);
    TextPos tmp(textPos_, 1);
    for(quint32 i = textPos_+1; i < textPos_+textLength; i++) {
        if (isSelected(i)) {
            if (selected) {
                tmp.length++;
            }
            else {
                setTextFormat(tmp.pos, tmp.length, m_hightLightFormat);
                tmp.pos = i;
                tmp.length = 1;
                selected = true;
            }
        }
        else {
            if (selected) {
                setTextFormat(tmp.pos, tmp.length, mergeFormat);
                tmp.pos = i;
                tmp.length = 1;
                selected = false;
            }
            else {
                tmp.length++;
            }
        }
    }

    if (selected) {
        setTextFormat(tmp.pos, tmp.length, mergeFormat);
    }
    else {
        setTextFormat(tmp.pos, tmp.length, m_hightLightFormat);
    }

    return true;
}

/*
 *  @Param:
 *      pos:        数据的位置
 *      length:     数据的长度
 */
bool CFrameShowWidget::removeHighLight(quint32 pos, quint32 length)
{
    bool bFind = false;
    quint32 textPos_ = textPos(pos);
    quint32 textLength = 2;
    if (length > 1) {
        textLength += textPos(pos+length-1) - textPos_;
    }
    if (textPos_ >= m_text.size() || textPos_+textLength > m_text.size()) {
        return false;
    }

    Q_FOREACH(const TextPos &highLightPos, m_highLightPos) {
        if (textPos_ == highLightPos.pos && textLength == highLightPos.length)
            bFind = true;
    }

    if(!bFind) {
        return false;
    }

    TextPos tmp(textPos_, 1);
    bool selected = isSelected(textPos_);
    for(quint32 i = textPos_+1; i < textPos_+textLength; i++) {
        if (isSelected(i)) {
            if (selected) {
                tmp.length++;
            }
            else {
                setTextFormat(tmp.pos, tmp.length, m_defaultFormat);
                tmp.pos = i;
                tmp.length = 1;
                selected = true;
            }
        }
        else {
            if (selected) {
                setTextFormat(tmp.pos, tmp.length, m_selectedFormat);
                tmp.pos = i;
                tmp.length = 1;
                selected = false;
            }
            else {
                tmp.length++;
            }
        }
    }
    if (selected) {
        setTextFormat(tmp.pos, tmp.length, m_selectedFormat);
    }
    else {
        setTextFormat(tmp.pos, tmp.length, m_defaultFormat);
    }

    m_highLightPos.removeOne(TextPos(textPos_, textLength));

    return true;
}

/*
 *  @Param:
 *      pos:        数据的位置
 *      length:     数据的长度
 */
bool CFrameShowWidget::setSelected(quint32 pos, quint32 length)
{
    quint32 textPos_ = textPos(pos);
    quint32 textLength = 2;
    if (length > 1) {
        textLength += textPos(pos+length-1) - textPos_;
    }
    if (textPos_ >= m_text.size() || textPos_+textLength > m_text.size()) {
        return false;
    }

    QTextCharFormat mergeFormat = m_hightLightFormat;
    mergeFormat.merge(m_selectedFormat);

    TextPos tmp(textPos_, 1);
    bool highLight = isHighLihgt(pos);
    for (quint32 i = textPos_+1; i < textPos_+textLength; i++) {
        if (isHighLihgt(i)) {
            if (highLight) {
                tmp.length++;
            }
            else {
                setTextFormat(tmp.pos, tmp.length, m_selectedFormat);
                tmp.pos = i;
                tmp.length = 1;
                highLight = true;
            }
        }
        else {
            if (highLight) {
                setTextFormat(tmp.pos, tmp.length, mergeFormat);
                tmp.pos = i;
                tmp.length = 1;
                highLight = false;
            }
            else {
                tmp.length++;
            }
        }
    }
    if (highLight) {
        setTextFormat(tmp.pos, tmp.length, mergeFormat);
    }
    else {
        setTextFormat(tmp.pos, tmp.length, m_selectedFormat);
    }

    m_hasSelectedText = true;
    m_selectedPos.pos = textPos_;
    m_selectedPos.length = textLength;

    return m_hasSelectedText;
}

void CFrameShowWidget::clearSelected()
{
    if (m_hasSelectedText) 
    {
        TextPos tmp(m_selectedPos.pos, 1);
        bool highLight = isHighLihgt(m_selectedPos.pos);
        for (quint32 i = m_selectedPos.pos+1; i < m_selectedPos.pos+m_selectedPos.length; i++) {
            if (isHighLihgt(i)) {
                if (highLight) {
                    tmp.length++;
                }
                else {
                    setTextFormat(tmp.pos, tmp.length, m_defaultFormat);
                    tmp.pos = i;
                    tmp.length = 1;
                    highLight = true;
                }
            }
            else {
                if (highLight) {
                    setTextFormat(tmp.pos, tmp.length, m_hightLightFormat);
                    tmp.pos = i;
                    tmp.length = 1;
                    highLight = false;
                }
                else {
                    tmp.length++;
                }
            }
        }
        if (highLight) {
            setTextFormat(tmp.pos, tmp.length, m_hightLightFormat);
        }
        else {
            setTextFormat(tmp.pos, tmp.length, m_defaultFormat);
        }

        m_hasSelectedText = false;
    }
}

//只能更新已有报文的内容，不能追加长度
void CFrameShowWidget::updateFrame(quint32 pos, quint32 length, const quint8 *frame)
{
    quint32 textPos_ = textPos(pos);
    QString strData;
    while(textPos_ < m_text.size() && length>0) {
        strData = QString("%1").arg(*frame, 2, 16, QChar('0')).toUpper();
        updateText(textPos_, strData);
        textPos_ = textPos(++pos);
        length--;
        frame++;
    }
}

void CFrameShowWidget::_init()
{
    this->setText(m_text);
    this->setReadOnly(true);

    m_defaultFormat.setFontWeight(FONT_WEIGHT);

    m_hightLightFormat = m_defaultFormat;
    m_hightLightFormat.setBackground(Qt::yellow);
    m_hightLightFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    m_hightLightFormat.setUnderlineColor(Qt::red);
    m_hightLightFormat.setForeground(Qt::red);

    m_selectedFormat = m_defaultFormat;
    m_selectedFormat.setBackground(Qt::black);
    m_selectedFormat.setForeground(Qt::white);

    setDefaultFormat();
}

void CFrameShowWidget::frameToString(const quint8 *frame_, quint32 length_, QString &text_)
{
    text_ = s_headerItem;
    text_ += '\n';
    text_ += s_headerSplit;
    text_ += '\n';

    int rows = 0;
    for(quint32 i = 0; i < length_; i+=ROW_FRAME_LENGTH)
    {
        quint32 len = (length_>i+ROW_FRAME_LENGTH) ? ROW_FRAME_LENGTH : length_-i;
        for(quint32 j = 0; j < len; j++) {
            text_ += QString("%1").arg(frame_[i+j], 2, 16, QChar('0')).toUpper();
            if (j+1 != len)
                text_ += ' ';
            if (j == 7) 
                text_ += ' ';
        }
        rows++;
        if (length_ > i+len) {
            text_ += '\n';
            if (rows%BLOCK_ROWS == 0) {
                text_ += '\n';
            }
        }

    }
}

quint32 CFrameShowWidget::textPos(quint32 framePos_)
{
    quint32 pos = HEADER_LENGTH;

    while(framePos_) {
        if (framePos_ >= BLOCK_FRAME_LENGTH) {
            pos += BLOCK_LENGTH;
            framePos_ -= BLOCK_FRAME_LENGTH;
        }
        else if (framePos_ >= ROW_FRAME_LENGTH) {
            pos += ROW_LENGTH;
            framePos_ -= ROW_FRAME_LENGTH;
        }
        else if (framePos_ >= ROW_FRAME_LENGTH/2) {
            pos += DATA_LENGTH*ROW_FRAME_LENGTH/2+1;
            framePos_ -= ROW_FRAME_LENGTH/2;
        }
        else {
            pos += DATA_LENGTH;
            framePos_ -= 1;
        }
    }

    return pos;
}

void CFrameShowWidget::setTextFormat(quint32 pos, quint32 length, const QTextCharFormat &format)
{
    if (length) {
        QTextCursor cursor = this->textCursor();
        cursor.movePosition( QTextCursor::Start );                              //  行首
        cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, pos); //  向右移动到Pos
        cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, length);
        cursor.setCharFormat(format);
        cursor.movePosition( QTextCursor::PreviousCharacter );//加上这句是为了去除光标selected
        this->setTextCursor( cursor ); 
    }
}

void CFrameShowWidget::setDefaultFormat()
{
    setTextFormat(0, m_text.size(), m_defaultFormat);
}

void CFrameShowWidget::updateText(quint32 pos, const QString& text_)
{
    QTextCursor cursor = this->textCursor();
    cursor.movePosition( QTextCursor::Start );                              //  行首
    cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, pos); //  向右移动到Pos
    cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, text_.length());
    QTextCharFormat format = cursor.charFormat();
    cursor.insertText(text_, format);
    cursor.movePosition( QTextCursor::PreviousCharacter );//加上这句是为了去除光标selected
    this->setTextCursor( cursor ); 
}

void CFrameShowWidget::mergeTextPos(QList<TextPos> &list)
{
    TextPos cur = list[0];
    for(int i = 1; i < list.size(); i++) {
        TextPos latest = list[i];
        if (latest.pos < cur.pos+cur.length) {
            if (latest.pos+latest.length <= cur.pos+cur.length) {   //后一个在前一个之内
                list.removeAt(i);
                i--;
            }
            else {
                int tmp = cur.pos+cur.length-latest.pos;
                latest.pos += tmp;
                latest.length -= tmp;
                list[i] = latest;
                cur = latest;
            }
        }
    }
}