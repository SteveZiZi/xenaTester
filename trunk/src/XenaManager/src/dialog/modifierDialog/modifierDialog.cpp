#include "modifierDialog.h"


#include <QtGui>

#define EDIT_WIDTH  50

#define MAX_STEP            15
#define MAX_REPEAT_NUM      4095

CModifierDialog::CModifierDialog(const QStandardItem &item, QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/)
: QDialog(parent, f)
{
    bool ok = false;;
    m_segInfo.data = item.data().toUInt(&ok);
    if (!ok || m_segInfo.length + m_segInfo.pos >= 1518) {
        m_segInfo.pos = 0;
        m_segInfo.length = 1024;
    }

    init();
}

CModifierDialog::CModifierDialog(eth_segment_info segInf, QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/)
: QDialog(parent, f)
{
    m_segInfo = segInf;
    if (m_segInfo.length + m_segInfo.pos >= 1518) {
        m_segInfo.pos = 0;
        m_segInfo.length = 1024;
    }
    init();
}


CModifierDialog::CModifierDialog(const ModifierData & data, const eth_segment_info &segInf, QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/)
: QDialog(parent, f)
{
    m_data = data;
    m_segInfo.data = segInf.data;

    resize(270, 170);
    _setupUI();
    _connectSignals();
}


CModifierDialog::~CModifierDialog()
{

}


void CModifierDialog::init()
{
    m_data.minVal = 1;
    m_data.maxVal = 65535;
    m_data.step = 1;
    m_data.repeatCnt = 1;
    m_data.type = INCREMENT;
    m_data.bitsNum = MODIFY_16BITS;
    m_data.pos = m_segInfo.pos;

    resize(270, 170);
    _setupUI();
    _connectSignals();
}

void CModifierDialog::_setupUI()
{
    this->setWindowTitle(tr("SetModifier"));

    QLabel* labelPos = new QLabel(tr("Position:"));
    m_editPos =new QLineEdit(QString::number(m_data.pos-m_segInfo.pos), this);

    QLabel* labelMin = new QLabel(tr("MinValue:"));
    m_editMin = new QLineEdit(QString::number(m_data.minVal), this);
    QLabel* labelMax = new QLabel(tr("MaxValue:"));
    m_editMax = new QLineEdit(QString::number(m_data.maxVal), this);
    QLabel* labelStep = new QLabel(tr("StepValue:"));
    m_editStep = new QLineEdit(QString::number(m_data.step), this);

    QLabel* labelRepeat = new QLabel(tr("LoopTimes:"));
    m_editRepeat = new QLineEdit(QString::number(m_data.repeatCnt), this);

    QLabel* labelType = new QLabel(tr("ChangeStyle:"));
    m_comboType = new QComboBox(this);
    m_comboType->addItem(tr("Increase"));
    m_comboType->addItem(tr("Decrease"));
    m_comboType->addItem(tr("Random"));
    m_comboType->setCurrentIndex(m_data.type);
    QLabel* labelBits = new QLabel(tr("ChangedBits"));
    m_comboBits = new QComboBox(this);
    m_comboBits->addItem(tr("8Bits"));
    m_comboBits->addItem(tr("16Bits"));
    m_comboBits->setCurrentIndex(m_data.bitsNum);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QVBoxLayout *label1 = new QVBoxLayout();
    label1->addWidget(labelPos);
    label1->addWidget(labelMin);
    label1->addWidget(labelMax);
    label1->addWidget(labelStep);
    
    QVBoxLayout *value1 = new QVBoxLayout();
    value1->addWidget(m_editPos);
    value1->addWidget(m_editMin);
    value1->addWidget(m_editMax);
    value1->addWidget(m_editStep);

    QVBoxLayout *label2 = new QVBoxLayout();
    label2->addWidget(labelType);
    label2->addWidget(labelBits);
    label2->addWidget(labelRepeat);

    QVBoxLayout *value2 = new QVBoxLayout();
    value2->addWidget(m_comboType);
    value2->addWidget(m_comboBits);
    value2->addWidget(m_editRepeat);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addLayout(label2);
    hLayout->addLayout(value2);
    hLayout->addLayout(label1);
    hLayout->addLayout(value1);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(buttonBox);

    this->setLayout(vLayout);
}

void CModifierDialog::_connectSignals()
{
    connect(m_editMin, SIGNAL(textEdited(const QString &)), this, SLOT(OnMinEdit(const QString &)));
    connect(m_editMin, SIGNAL(editingFinished()), this, SLOT(OnMinEditFinished()));

    connect(m_editMax, SIGNAL(textEdited(const QString &)), this, SLOT(OnMaxEdit(const QString &)));
    connect(m_editMax, SIGNAL(editingFinished()), this, SLOT(OnMaxEditFinished()));

    connect(m_editStep, SIGNAL(textEdited(const QString &)), this, SLOT(OnStepEdit(const QString &)));
    connect(m_editStep, SIGNAL(editingFinished()), this, SLOT(OnStepEditFinished()));

    connect(m_editRepeat, SIGNAL(textEdited(const QString &)), this, SLOT(OnRepeatEdit(const QString &)));
    connect(m_editRepeat, SIGNAL(editingFinished()), this, SLOT(OnRepeatEditFinished()));

    connect(m_editPos, SIGNAL(textEdited(const QString &)), this, SLOT(OnPosEdit(const QString &)));
    connect(m_editPos, SIGNAL(editingFinished()), this, SLOT(OnPosEditFinished()));

    connect(m_comboType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTypeIndexChanged(int)));
    connect(m_comboBits, SIGNAL(currentIndexChanged(int)), this, SLOT(OnBitsIndexChanged(int)));

}


void CModifierDialog::OnMinEdit(const QString & text)
{
    quint32 val = text.toUInt();

    if (val >= m_data.maxVal) {
        m_editMin->setStyleSheet("color:red");
    }
    else {
        m_editMin->setStyleSheet("color:black");
    }
}
void CModifierDialog::OnMinEditFinished()
{
    quint32 val = m_editMin->text().toUInt();

    if (val >= m_data.maxVal) {
        m_editMin->setText(QString::number(m_data.minVal));
        m_editMin->setStyleSheet("color:black");
    }
    else {
        m_data.minVal = val;
    }
}

void CModifierDialog::OnMaxEdit(const QString & text)
{
    quint32 val = text.toUInt();
    quint32 maxVal = 65535;
    if (m_data.bitsNum == MODIFY_8BITS) {
        maxVal = 255;
    }
    if (val > maxVal) {
        m_editMax->setStyleSheet("color:red");
    }
    else {
        m_editMax->setStyleSheet("color:black");
    }
}
void CModifierDialog::OnMaxEditFinished()
{
    quint32 val = m_editMax->text().toUInt();
    quint32 maxVal = 65535;
    if (m_data.bitsNum == MODIFY_8BITS) {
        maxVal = 255;
    }

    if (val > maxVal) {
        m_editMax->setText(QString::number(m_data.maxVal));
        m_editMax->setStyleSheet("color:black");
    }
    else {
        m_data.maxVal = val;
    }
}

void CModifierDialog::OnStepEdit(const QString & text)
{
    quint32 val = text.toUInt();

    if (val > MAX_STEP) {
        m_editStep->setStyleSheet("color:red");
    }
    else {
        m_editStep->setStyleSheet("color:black");
    }
}
void CModifierDialog::OnStepEditFinished()
{
    quint32 val = m_editStep->text().toUInt();

    if (val > MAX_STEP) {
        m_editStep->setText(QString::number(m_data.step));
        m_editStep->setStyleSheet("color:black");
    }
    else {
        m_data.step = val;
    }
}

void CModifierDialog::OnRepeatEdit(const QString &text)
{
    quint32 val = text.toUInt();

    if (val > MAX_REPEAT_NUM) {
        m_editRepeat->setStyleSheet("color:red");
    }
    else {
        m_editRepeat->setStyleSheet("color:black");
    }
}
void CModifierDialog::OnRepeatEditFinished()
{
    quint32 val = m_editRepeat->text().toUInt();

    if (val > MAX_REPEAT_NUM) {
        m_editRepeat->setText(QString::number(m_data.repeatCnt));
        m_editRepeat->setStyleSheet("color:black");
    }
    else {
        m_data.repeatCnt = val;
    }
}

void CModifierDialog::OnPosEdit(const QString &text)
{
    quint32 val = text.toUInt();
    quint32 maxVal = m_segInfo.length-1;
    if (m_data.bitsNum == MODIFY_16BITS) {
        maxVal--;
    }

    if (val > maxVal) {
        m_editPos->setStyleSheet("color:red");
    }
    else {
        m_editPos->setStyleSheet("color:black");
    }
}
void CModifierDialog::OnPosEditFinished()
{
    quint32 val = m_editPos->text().toUInt();
    quint32 maxVal = m_segInfo.length-1;
    if (m_data.bitsNum == MODIFY_16BITS) {
        maxVal--;
    }

    if (val > maxVal) {
        m_editPos->setText(QString::number(m_data.pos-m_segInfo.pos));
        m_editPos->setStyleSheet("color:black");
    }
    else {
        m_data.pos = m_segInfo.pos + val;
    }
}


void CModifierDialog::OnTypeIndexChanged(int index)
{
    if (index == RANDOM) {
        m_editMin->setEnabled(false);
        m_editMax->setEnabled(false);
        m_editStep->setEnabled(false);
        m_data.type = RANDOM;
    }
    else {
        m_editMin->setEnabled(true);
        m_editMax->setEnabled(true);
        m_editStep->setEnabled(true);
        if (index == INCREMENT) {
            m_data.type = INCREMENT;
        }
        else if (index == DECREMENT) {
            m_data.type = DECREMENT;
        }
    }
}

void CModifierDialog::OnBitsIndexChanged(int index)
{
    if (index == MODIFY_8BITS) {
        if (m_data.maxVal > 255) {
            m_data.maxVal = 255;
            m_editMax->setText("255");
        }
        if (m_data.minVal >= m_data.maxVal) {
            m_data.minVal = 1;
            m_editMin->setText("1");
        }
        if (m_data.pos >= m_segInfo.length-1) {
            m_data.pos = 0;
            m_editPos->setText("0");
        }
        m_data.bitsNum = MODIFY_8BITS;
    }
    else if (index == MODIFY_16BITS) {
        m_data.bitsNum = MODIFY_16BITS;
    }
}