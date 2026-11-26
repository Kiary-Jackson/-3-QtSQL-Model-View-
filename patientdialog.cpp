#include "patientdialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QRegularExpressionValidator>

PatientDialog::PatientDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("患者信息"));
    setMinimumWidth(420);
    buildUi();
}

void PatientDialog::buildUi()
{
    m_name = new QLineEdit(this);
    m_name->setPlaceholderText(QStringLiteral("必填，例如：张伟"));

    m_gender = new QComboBox(this);
    m_gender->addItems({QStringLiteral("男"), QStringLiteral("女"), QStringLiteral("其他")});

    m_age = new QSpinBox(this);
    m_age->setRange(0, 120);
    m_age->setSuffix(QStringLiteral(" 岁"));

    m_phone = new QLineEdit(this);
    m_phone->setPlaceholderText(QStringLiteral("例如：13800138000"));
    m_phone->setValidator(new QRegularExpressionValidator(
        QRegularExpression(QStringLiteral("[0-9+\\- ]{0,20}")), this));

    m_department = new QComboBox(this);
    m_department->setEditable(true);
    m_department->addItems({QStringLiteral("内科"), QStringLiteral("外科"),
                            QStringLiteral("儿科"), QStringLiteral("骨科"),
                            QStringLiteral("急诊科")});

    m_visitDate = new QDateEdit(this);
    m_visitDate->setCalendarPopup(true);
    m_visitDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    m_visitDate->setDate(QDate::currentDate());

    m_status = new QComboBox(this);
    m_status->addItems({QStringLiteral("待诊"), QStringLiteral("就诊中"),
                        QStringLiteral("已完成"), QStringLiteral("已取消")});

    m_note = new QPlainTextEdit(this);
    m_note->setPlaceholderText(QStringLiteral("选填，症状或备注"));
    m_note->setFixedHeight(70);

    m_tip = new QLabel(QStringLiteral("带 * 为必填项"), this);
    m_tip->setStyleSheet(QStringLiteral("color: #666;"));

    QFormLayout *form = new QFormLayout;
    form->addRow(QStringLiteral("* 姓名"), m_name);
    form->addRow(QStringLiteral("性别"), m_gender);
    form->addRow(QStringLiteral("年龄"), m_age);
    form->addRow(QStringLiteral("联系电话"), m_phone);
    form->addRow(QStringLiteral("科室"), m_department);
    form->addRow(QStringLiteral("就诊日期"), m_visitDate);
    form->addRow(QStringLiteral("状态"), m_status);
    form->addRow(QStringLiteral("备注"), m_note);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    if (QPushButton *okBtn = buttons->button(QDialogButtonBox::Ok))
        okBtn->setText(QStringLiteral("确定"));
    if (QPushButton *cancelBtn = buttons->button(QDialogButtonBox::Cancel))
        cancelBtn->setText(QStringLiteral("取消"));
    connect(buttons, &QDialogButtonBox::accepted, this, &PatientDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &PatientDialog::reject);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->addWidget(m_tip);
    root->addLayout(form);
    root->addWidget(buttons);
}

void PatientDialog::setValues(const QStringList &v)
{
    if (v.size() < 8) return;
    m_name->setText(v.at(0));
    const int gi = m_gender->findText(v.at(1));
    m_gender->setCurrentIndex(gi >= 0 ? gi : 0);
    m_age->setValue(v.at(2).toInt());
    m_phone->setText(v.at(3));
    m_department->setEditText(v.at(4));
    const QDate d = QDate::fromString(v.at(5), QStringLiteral("yyyy-MM-dd"));
    m_visitDate->setDate(d.isValid() ? d : QDate::currentDate());
    const int si = m_status->findText(v.at(6));
    m_status->setCurrentIndex(si >= 0 ? si : 0);
    m_note->setPlainText(v.at(7));
}

QStringList PatientDialog::values() const
{
    return QStringList()
           << m_name->text().trimmed()
           << m_gender->currentText()
           << QString::number(m_age->value())
           << m_phone->text().trimmed()
           << m_department->currentText().trimmed()
           << m_visitDate->date().toString(QStringLiteral("yyyy-MM-dd"))
           << m_status->currentText()
           << m_note->toPlainText().trimmed();
}

void PatientDialog::accept()
{
    // 数据完整性：必要字段校验
    if (m_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("输入不完整"),
                             QStringLiteral("姓名为必填项，请填写后再提交。"));
        m_name->setFocus();
        return;
    }
    if (m_phone->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("输入不完整"),
                             QStringLiteral("联系电话为必填项，请填写后再提交。"));
        m_phone->setFocus();
        return;
    }
    if (m_department->currentText().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("输入不完整"),
                             QStringLiteral("科室为必填项，请填写或选择。"));
        m_department->setFocus();
        return;
    }
    QDialog::accept();
}
