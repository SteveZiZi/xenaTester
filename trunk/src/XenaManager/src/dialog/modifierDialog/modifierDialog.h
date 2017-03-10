#pragma once

#include <QDialog>
#include "XenaManager/src/dialog/ImportData/utilTools.h"

class QLineEdit;
class QComboBox;
class QStandardItem;
class QDialogButtonBox;

class CModifierDialog : public QDialog
{
    Q_OBJECT
public:
    enum ModifyType {
        INCREMENT,              //递增
        DECREMENT,              //递减
        RANDOM                  //随机
    };

    enum ModifyBits {
        MODIFY_8BITS,           //只能修改8bit
        MODIFY_16BITS           //只能修改16bit
    };

    struct ModifierData {
        quint16 pos;            //在哪个位置递变，0->报文的第一个字节
        quint16 minVal;         //变化的最小值
        quint16 maxVal;         //变化的最大值
        quint16 step;           //变化的步长
        quint16 repeatCnt;      //变化循环次数

        ModifyType type;        //变化的类型
        ModifyBits bitsNum;     //变位的位数

        quint16 getPos() const {return pos;}
        void setPos(const quint16 pos_) {pos = pos_;}
        quint16 getMinVal() const {return minVal;}
        void setMinVal(const quint16 minVal_) {minVal = minVal_;}
        quint16 getMaxVal() const {return maxVal;}
        void setMaxVal(const quint16 maxVal_) {maxVal = maxVal_;}
        quint16 getStep() const {return step;}
        void setStep(const quint16 step_) {step = step_;}
        quint16 getRepeatCnt() const {return repeatCnt;}
        void setRepeatCnt(const quint16 repeatCnt_) {repeatCnt = repeatCnt_;}
        ModifyType getModifyType() const {return type;}
        void setModifyType(const QString & type_)
        {
            if (type_.contains("INC",Qt::CaseInsensitive))
            {
                type = INCREMENT;
            }
            else if (type_.contains("DEC",Qt::CaseInsensitive))
            {
                type = DECREMENT;
            }
            else if (type_.contains("RANDOM",Qt::CaseInsensitive))
            {
                type = RANDOM;
            }
        }
        void setModifyType(const ModifyType type_) {type = type_;}

        ModifyBits getBitsNum() const {return bitsNum;}
        void setBitsNum(const QString & bitNum_)
        {
            if (bitNum_.contains("0XFFFF0000",Qt::CaseInsensitive))
            {
                bitsNum = MODIFY_16BITS;
            }
            else
            {
                bitsNum = MODIFY_8BITS;
            }
        }

        void setBitsNum(const ModifyBits bitNum_) {bitsNum = bitNum_;}
    };

    CModifierDialog(const QStandardItem &item, QWidget *parent = 0, Qt::WindowFlags f = 0);
    CModifierDialog(eth_segment_info segInf, QWidget *parent = 0, Qt::WindowFlags f = 0);
    CModifierDialog(const ModifierData &data, const eth_segment_info &segInf, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~CModifierDialog();

    void modifierData(ModifierData &data_) {memcpy(&data_, &m_data, sizeof(ModifierData));}

private slots:
    void OnMinEdit(const QString &);
    void OnMinEditFinished();
    void OnMaxEdit(const QString &);
    void OnMaxEditFinished();
    void OnStepEdit(const QString &);
    void OnStepEditFinished();
    void OnRepeatEdit(const QString &);
    void OnRepeatEditFinished();

    void OnPosEdit(const QString &);
    void OnPosEditFinished();

    void OnTypeIndexChanged(int);
    void OnBitsIndexChanged(int);

private:
    void init();
    void _setupUI();
    void _connectSignals();

private:
    ModifierData m_data;

    QLineEdit *m_editMin;
    QLineEdit *m_editMax;
    QLineEdit *m_editStep;
    QLineEdit *m_editRepeat;
    QLineEdit *m_editPos;
    QComboBox *m_comboType;
    QComboBox *m_comboBits;

    QDialogButtonBox *buttonBox;

    eth_segment_info m_segInfo;
};