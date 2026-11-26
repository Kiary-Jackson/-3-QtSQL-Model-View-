#include "dbmanager.h"
#include "config.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include <utility>

namespace {
const char *kConnectionName = "hospital_sqlite_conn";
}

DBManager::DBManager(QObject *parent)
    : QObject(parent)
{
    // 数据库放在可执行文件目录，避免"运行目录不同导致找不到库"的问题
    m_databasePath = QCoreApplication::applicationDirPath()
                     + QDir::separator() + AppConfig::DatabaseFile;
}

DBManager *DBManager::instance()
{
    static DBManager inst;
    return &inst;
}

bool DBManager::open(QString *error)
{
    if (m_db.isValid() && m_db.isOpen())
        return true;

    if (!QSqlDatabase::drivers().contains(QStringLiteral("QSQLITE"))) {
        m_lastError = QStringLiteral("当前 Qt 构建未包含 QSQLITE 驱动，请安装 Qt Base 的 SQL SQLite 支持。");
        if (error) *error = m_lastError;
        return false;
    }

    if (QSqlDatabase::contains(kConnectionName))
        m_db = QSqlDatabase::database(kConnectionName);
    else
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), kConnectionName);

    const QFileInfo dbFile(m_databasePath);
    if (dbFile.absoluteDir().exists() == false) {
        m_lastError = QStringLiteral("数据库目录不存在：%1").arg(dbFile.absoluteDir().path());
        if (error) *error = m_lastError;
        return false;
    }

    m_db.setDatabaseName(m_databasePath);
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        if (error) *error = m_lastError;
        return false;
    }

    return initSchema(error);
}

bool DBManager::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

QSqlDatabase DBManager::database() const
{
    return m_db;
}

QString DBManager::databasePath() const
{
    return m_databasePath;
}

bool DBManager::exec(const QString &sql,
                     const QVariantList &params,
                     QSqlQuery *outQuery,
                     QString *error)
{
    QSqlQuery query(m_db);
    if (!query.prepare(sql)) {
        m_lastError = QStringLiteral("SQL 预处理失败：%1 | SQL：%2")
                          .arg(query.lastError().text(), sql);
        if (error) *error = m_lastError;
        return false;
    }
    for (const QVariant &v : params)
        query.addBindValue(v);

    if (!query.exec()) {
        m_lastError = QStringLiteral("SQL 执行失败：%1 | SQL：%2")
                          .arg(query.lastError().text(), sql);
        if (error) *error = m_lastError;
        return false;
    }
    if (outQuery)
        *outQuery = std::move(query);
    return true;
}

bool DBManager::initSchema(QString *error)
{
    const QString sqlUsers = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS users ("
        "username TEXT PRIMARY KEY, "
        "password TEXT NOT NULL, "
        "role TEXT NOT NULL DEFAULT 'doctor'"
        ")");
    const QString sqlPatients = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS patients ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "gender TEXT NOT NULL DEFAULT '', "
        "age INTEGER NOT NULL DEFAULT 0, "
        "phone TEXT NOT NULL DEFAULT '', "
        "department TEXT NOT NULL DEFAULT '', "
        "visit_date TEXT NOT NULL DEFAULT '', "
        "status TEXT NOT NULL DEFAULT '', "
        "note TEXT NOT NULL DEFAULT ''"
        ")");

    if (!exec(sqlUsers, QVariantList(), nullptr, error)) return false;
    if (!exec(sqlPatients, QVariantList(), nullptr, error)) return false;

    // 没有账号时写入默认管理员（首次运行）
    QSqlQuery q;
    if (!exec(QStringLiteral("SELECT COUNT(*) FROM users"), QVariantList(), &q, error))
        return false;
    if (q.next() && q.value(0).toInt() == 0) {
        if (!exec(QStringLiteral("INSERT INTO users(username, password, role) VALUES(?, ?, ?)"),
                  QVariantList() << AppConfig::DefaultUser
                                 << AppConfig::DefaultPassword
                                 << QStringLiteral("管理员"),
                  nullptr, error))
            return false;
    }
    return true;
}
