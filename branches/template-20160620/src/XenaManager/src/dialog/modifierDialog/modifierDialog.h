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
        INCREMENT,              //����
        DECREMENT,              //�ݼ�
        RANDOM                  //���
    };

    enum ModifyBits {
        MODIFY_8BITS,           //ֻ���޸�8bit
        MODIFY_16BITS           //ֻ���޸�16bit
    };

    struct ModifierData {
        quint16 pos;            //���ĸ�λ�õݱ䣬0->���ĵĵ�һ���ֽ�
        quint16 minVal;         //�仯����Сֵ
        quint16 maxVal;         //�仯�����ֵ
        quint16 step;           //�仯�Ĳ���
        quint16 repeatCnt;      //�仯ѭ������

        ModifyType type;        //�仯������
        ModifyBits bitsNum;     //��λ��λ��
    };

    CModifierDialog(const QStandardItem &item, QWidget *parent = 0, Qt::WindowFlags f = 0);
    CModifierDialog(eth_segment_info segInf, QWidget *parent = 0, Qt::WindowFlags f = 0);
    CModifierDialog(const ModifierData &data, QWidget *parent = 0, Qt::WindowFlags f = 0);
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