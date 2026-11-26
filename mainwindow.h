#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class LoginWidget;
class PatientWidget;

// 主窗口：用 QStackedWidget 在登录页与患者管理页之间切换
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onLoginSucceeded(const QString &user);
    void onLogout();

private:
    QStackedWidget *m_stack;
    LoginWidget    *m_loginPage;
    PatientWidget  *m_patientPage;
};

#endif // MAINWINDOW_H
