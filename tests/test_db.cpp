// 数据层验证程序：复用与 GUI 完全相同的 DBManager / PatientModel，
// 验证"新增→修改→查询→删除→刷新"的数据库实际结果一致性。
#include "../dbmanager.h"
#include "../patientmodel.h"

#include <QCoreApplication>
#include <QSqlQuery>
#include <QTextStream>

static QTextStream out(stdout);
static int failures = 0;

static void check(bool cond, const QString &caseName, const QString &detail = QString())
{
    out << (cond ? "[PASS] " : "[FAIL] ") << caseName;
    if (!detail.isEmpty()) out << "  -> " << detail;
    out << "\n";
    if (!cond) ++failures;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString err;
    check(DBManager::instance()->open(&err), QStringLiteral("数据库连接与建表"), err);
    if (!DBManager::instance()->isOpen()) return 1;

    // 清空，保证测试可重复
    DBManager::instance()->exec(QStringLiteral("DELETE FROM patients"));

    PatientModel model;
    const QStringList row1 = {QStringLiteral("张伟"), QStringLiteral("男"),
                              QStringLiteral("35"), QStringLiteral("13800138001"),
                              QStringLiteral("内科"), QStringLiteral("2026-11-26"),
                              QStringLiteral("待诊"), QStringLiteral("测试记录A")};
    const QStringList row2 = {QStringLiteral("李娜"), QStringLiteral("女"),
                              QStringLiteral("28"), QStringLiteral("13800138002"),
                              QStringLiteral("儿科"), QStringLiteral("2026-11-25"),
                              QStringLiteral("已完成"), QStringLiteral("测试记录B")};

    check(model.addPatient(row1, &err), QStringLiteral("新增记录1"), err);
    check(model.addPatient(row2, &err), QStringLiteral("新增记录2"), err);
    check(model.rowCount() == 2, QStringLiteral("新增后视图行数=2"),
          QStringLiteral("实际=%1").arg(model.rowCount()));

    // 用 QSqlQuery 直接读数据库，验证视图与数据库一致
    QSqlQuery q;
    DBManager::instance()->exec(QStringLiteral("SELECT COUNT(*) FROM patients"),
                                QVariantList(), &q, &err);
    const int dbCount = q.next() ? q.value(0).toInt() : -1;
    check(dbCount == 2, QStringLiteral("数据库中实际记录数=2"),
          QStringLiteral("实际=%1").arg(dbCount));

    // 通用查询：命中 / 未命中
    check(model.search(QStringLiteral("张伟"), &err) && model.rowCount() == 1,
          QStringLiteral("通用查询命中1条"), err);
    check(model.search(QStringLiteral("不存在的人"), &err) && model.rowCount() == 0,
          QStringLiteral("查询无结果返回0条"));
    model.clearSearch();
    model.reload();

    // 修改：把第 0 行姓名改为"张伟改"，并验证数据库落库
    QStringList edited = row1;
    edited[0] = QStringLiteral("张伟改");
    const int id = model.recordId(0);
    check(model.updatePatient(0, edited, &err), QStringLiteral("修改记录"), err);
    QSqlQuery q2;
    DBManager::instance()->exec(QStringLiteral("SELECT name FROM patients WHERE id=?"),
                                QVariantList() << id, &q2, &err);
    const QString dbName = q2.next() ? q2.value(0).toString() : QString();
    check(dbName == QStringLiteral("张伟改"), QStringLiteral("修改后数据库姓名一致"),
          QStringLiteral("数据库=%1").arg(dbName));

    // 删除：确认数据库实际删除
    check(model.removePatient(0, &err), QStringLiteral("删除记录"), err);
    QSqlQuery q3;
    DBManager::instance()->exec(QStringLiteral("SELECT COUNT(*) FROM patients WHERE id=?"),
                                QVariantList() << id, &q3, &err);
    const int remaining = q3.next() ? q3.value(0).toInt() : -1;
    check(remaining == 0, QStringLiteral("删除后数据库无该记录"),
          QStringLiteral("剩余=%1").arg(remaining));
    check(model.rowCount() == 1, QStringLiteral("删除后视图行数=1"),
          QStringLiteral("实际=%1").arg(model.rowCount()));

    // 刷新一致性
    DBManager::instance()->exec(QStringLiteral("INSERT INTO patients(name) VALUES('外部写入')"));
    check(model.reload(&err) && model.rowCount() == 2,
          QStringLiteral("刷新后反映外部写入"), err);

    out << "\n==== 结果: " << (failures == 0 ? QStringLiteral("全部通过")
                                            : QStringLiteral("失败 %1 项").arg(failures))
        << " ====\n";
    return failures == 0 ? 0 : 1;
}
