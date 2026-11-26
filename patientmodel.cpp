#include "patientmodel.h"
#include "dbmanager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {
// 新增/修改时写入的字段顺序，必须与 PatientColumn 的 1..8 对应
const QStringList kWritableFields = {
    QStringLiteral("name"),
    QStringLiteral("gender"),
    QStringLiteral("age"),
    QStringLiteral("phone"),
    QStringLiteral("department"),
    QStringLiteral("visit_date"),
    QStringLiteral("status"),
    QStringLiteral("note")
};
}

PatientModel::PatientModel(QObject *parent)
    : QSqlTableModel(parent, DBManager::instance()->database())
{
    setTable(QStringLiteral("patients"));
    setEditStrategy(QSqlTableModel::OnManualSubmit);

    setHeaderData(PC_Id,         Qt::Horizontal, QStringLiteral("编号"));
    setHeaderData(PC_Name,       Qt::Horizontal, QStringLiteral("姓名"));
    setHeaderData(PC_Gender,     Qt::Horizontal, QStringLiteral("性别"));
    setHeaderData(PC_Age,        Qt::Horizontal, QStringLiteral("年龄"));
    setHeaderData(PC_Phone,      Qt::Horizontal, QStringLiteral("联系电话"));
    setHeaderData(PC_Department, Qt::Horizontal, QStringLiteral("科室"));
    setHeaderData(PC_VisitDate,  Qt::Horizontal, QStringLiteral("就诊日期"));
    setHeaderData(PC_Status,     Qt::Horizontal, QStringLiteral("状态"));
    setHeaderData(PC_Note,       Qt::Horizontal, QStringLiteral("备注"));
}

bool PatientModel::reload(QString *error)
{
    if (!select()) {
        m_lastError = QSqlTableModel::lastError().text();
        if (error) *error = m_lastError;
        return false;
    }
    return true;
}

bool PatientModel::search(const QString &keyword, QString *error)
{
    const QString kw = keyword.trimmed();
    if (kw.isEmpty()) {
        clearSearch();
        return reload(error);
    }

    // QSqlQuery 通用查询：一次 LIKE 匹配多个字段，参数绑定防止注入
    const QString sql = QStringLiteral(
        "SELECT id FROM patients "
        "WHERE name LIKE ? OR phone LIKE ? OR department LIKE ? OR status LIKE ? OR note LIKE ? "
        "ORDER BY id");
    const QString pattern = QStringLiteral("%%1%").arg(kw);
    QSqlQuery q;
    if (!DBManager::instance()->exec(sql,
                                     QVariantList() << pattern << pattern << pattern
                                                    << pattern << pattern,
                                     &q, error)) {
        m_lastError = DBManager::instance()->lastError();
        if (error) *error = m_lastError;
        return false;
    }

    QStringList ids;
    while (q.next())
        ids << q.value(0).toString();

    if (ids.isEmpty())
        setFilter(QStringLiteral("1=0"));              // 查询无结果：过滤为空集
    else
        setFilter(QStringLiteral("id IN (%1)").arg(ids.join(QStringLiteral(","))));

    return reload(error);
}

void PatientModel::clearSearch()
{
    setFilter(QString());
}

bool PatientModel::addPatient(const QStringList &values, QString *error)
{
    if (values.size() != kWritableFields.size()) {
        m_lastError = QStringLiteral("字段数量不匹配，无法新增记录。");
        if (error) *error = m_lastError;
        return false;
    }

    QStringList placeholders;
    for (int i = 0; i < kWritableFields.size(); ++i)
        placeholders << QStringLiteral("?");
    const QString sql = QStringLiteral("INSERT INTO patients(%1) VALUES(%2)")
                            .arg(kWritableFields.join(QStringLiteral(",")),
                                 placeholders.join(QStringLiteral(",")));

    QVariantList params;
    for (const QString &v : values)
        params << v;

    if (!DBManager::instance()->exec(sql, params, nullptr, error)) {
        m_lastError = DBManager::instance()->lastError();
        if (error) *error = m_lastError;
        return false;
    }
    return reload(error);   // 立刻刷新，保证视图与数据库一致
}

bool PatientModel::updatePatient(int row, const QStringList &values, QString *error)
{
    const int id = recordId(row);
    if (id <= 0) {
        m_lastError = QStringLiteral("未选中有效的患者记录，无法修改。");
        if (error) *error = m_lastError;
        return false;
    }
    if (values.size() != kWritableFields.size()) {
        m_lastError = QStringLiteral("字段数量不匹配，无法修改记录。");
        if (error) *error = m_lastError;
        return false;
    }

    QStringList sets;
    QVariantList params;
    for (int i = 0; i < kWritableFields.size(); ++i) {
        sets << QStringLiteral("%1=?").arg(kWritableFields.at(i));
        params << values.at(i);
    }
    params << id;
    const QString sql = QStringLiteral("UPDATE patients SET %1 WHERE id=?")
                            .arg(sets.join(QStringLiteral(",")));

    if (!DBManager::instance()->exec(sql, params, nullptr, error)) {
        m_lastError = DBManager::instance()->lastError();
        if (error) *error = m_lastError;
        return false;
    }
    return reload(error);
}

bool PatientModel::removePatient(int row, QString *error)
{
    const int id = recordId(row);
    if (id <= 0) {
        m_lastError = QStringLiteral("未选中有效的患者记录，无法删除。");
        if (error) *error = m_lastError;
        return false;
    }
    if (!DBManager::instance()->exec(QStringLiteral("DELETE FROM patients WHERE id=?"),
                                     QVariantList() << id, nullptr, error)) {
        m_lastError = DBManager::instance()->lastError();
        if (error) *error = m_lastError;
        return false;
    }
    return reload(error);
}

QString PatientModel::displayName(int row) const
{
    if (row < 0 || row >= rowCount())
        return QString();
    return data(index(row, PC_Name), Qt::DisplayRole).toString();
}

int PatientModel::recordId(int row) const
{
    if (row < 0 || row >= rowCount())
        return 0;
    return data(index(row, PC_Id), Qt::EditRole).toInt();
}
