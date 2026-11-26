#ifndef PATIENTMODEL_H
#define PATIENTMODEL_H

#include <QSqlTableModel>
#include <QString>

// 患者表在视图中的列序号（与 patients 表字段顺序一致）
enum PatientColumn {
    PC_Id = 0,
    PC_Name,
    PC_Gender,
    PC_Age,
    PC_Phone,
    PC_Department,
    PC_VisitDate,
    PC_Status,
    PC_Note,
    PC_Count
};

// Model 层：封装患者表的所有数据操作，View 层不直接写 SQL
class PatientModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit PatientModel(QObject *parent = nullptr);

    bool reload(QString *error = nullptr);

    // 通用查询：用 QSqlQuery 执行参数化 LIKE 查询，再把结果 id 交给模型过滤
    bool search(const QString &keyword, QString *error = nullptr);
    void clearSearch();

    // 增 / 改 / 删：参数化 SQL + 强制 select() 保证数据库与视图一致
    bool addPatient(const QStringList &values, QString *error = nullptr);
    bool updatePatient(int row, const QStringList &values, QString *error = nullptr);
    bool removePatient(int row, QString *error = nullptr);

    QString displayName(int row) const;
    int recordId(int row) const;
    QString lastError() const { return m_lastError; }

private:
    QString m_lastError;
};

#endif // PATIENTMODEL_H
