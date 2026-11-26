#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

// ---------------------------------------------------------------
// 在这里修改成你自己的信息（会显示在登录页与患者管理界面）
// ---------------------------------------------------------------
namespace AppConfig {
static const QString DeveloperName    = QStringLiteral("冯凯源");
static const QString DeveloperStudentId = QStringLiteral("2024414290307");

// 默认管理员账号（数据库初始化时写入）
static const QString DefaultUser     = QStringLiteral("admin");
static const QString DefaultPassword = QStringLiteral("123456");

// SQLite 数据库文件名（生成在可执行文件所在目录）
static const QString DatabaseFile    = QStringLiteral("hospital.db");
} // namespace AppConfig

#endif // CONFIG_H
