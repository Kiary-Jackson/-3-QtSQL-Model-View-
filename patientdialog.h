#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QComboBox;
class QSpinBox;
class QDateEdit;
class QPlainTextEdit;
class QLabel;

// 新增 / 修改患者共用表单，含必要字段校验
class PatientDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PatientDialog(QWidget *parent = nullptr);

    void setValues(const QStringList &values);  // 顺序与 PatientColumn 1..8 一致
    QStringList values() const;

public slots:
    void accept() override;   // 校验通过才关闭

private:
    void buildUi();

    QLineEdit    *m_name;
    QComboBox    *m_gender;
    QSpinBox     *m_age;
    QLineEdit    *m_phone;
    QComboBox    *m_department;
    QDateEdit    *m_visitDate;
    QComboBox    *m_status;
    QPlainTextEdit *m_note;
    QLabel       *m_tip;
};

#endif // PATIENTDIALOG_H
