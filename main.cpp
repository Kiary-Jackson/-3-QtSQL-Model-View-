#include "mainwindow.h"
#include "dbmanager.h"
#include "config.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral("HospitalManagement"));
    a.setApplicationDisplayName(QStringLiteral("医院诊疗系统 - 用户信息管理"));

    QString dbError;
    if (!DBManager::instance()->open(&dbError)) {
        QMessageBox::critical(nullptr,
                              QStringLiteral("数据库连接失败"),
                              QStringLiteral("无法连接或初始化数据库：\n%1\n\n程序即将退出。")
                                  .arg(dbError));
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
