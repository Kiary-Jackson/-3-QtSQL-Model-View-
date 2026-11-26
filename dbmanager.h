#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QVariantList>

// 数据库管理单例：负责连接、建表、以及统一的 QSqlQuery 执行入口（通用查询）
class DBManager : public QObject
{
public:
    static DBManager *instance();

    bool open(QString *error = nullptr);
    bool isOpen() const;
    QSqlDatabase database() const;
    QString databasePath() const;
    QString lastError() const { return m_lastError; }

    // 通用查询/执行接口：参数化 SQL，返回 false 时可通过 error 或 lastError() 取原因
    bool exec(const QString &sql,
              const QVariantList &params = QVariantList(),
              QSqlQuery *outQuery = nullptr,
              QString *error = nullptr);

    DBManager(const DBManager &) = delete;
    DBManager &operator=(const DBManager &) = delete;

private:
    explicit DBManager(QObject *parent = nullptr);
    bool initSchema(QString *error = nullptr);

    QSqlDatabase m_db;
    QString m_databasePath;
    QString m_lastError;
};

#endif // DBMANAGER_H
