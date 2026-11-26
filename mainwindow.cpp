#include "mainwindow.h"
#include "loginwidget.h"
#include "patientwidget.h"
#include "config.h"

#include <QAction>
#include <QMenuBar>
#include <QStackedWidget>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("医院诊疗系统 - 用户信息管理"));
    resize(1000, 660);

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_loginPage = new LoginWidget(this);
    m_patientPage = new PatientWidget(this);
    m_stack->addWidget(m_loginPage);
    m_stack->addWidget(m_patientPage);
    m_stack->setCurrentWidget(m_loginPage);

    connect(m_loginPage, &LoginWidget::loginSucceeded,
            this, &MainWindow::onLoginSucceeded);

    // 代码化添加菜单与工具栏：退出登录
    QAction *logoutAction = new QAction(QStringLiteral("退出登录"), this);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::onLogout);

    QMenu *menu = menuBar()->addMenu(QStringLiteral("系统"));
    menu->addAction(logoutAction);
    QToolBar *toolBar = addToolBar(QStringLiteral("主工具栏"));
    toolBar->setMovable(false);
    toolBar->addAction(logoutAction);
}

void MainWindow::onLoginSucceeded(const QString &user)
{
    m_patientPage->setCurrentUser(user);
    m_stack->setCurrentWidget(m_patientPage);
    setWindowTitle(QStringLiteral("医院诊疗系统 - 用户信息管理（当前用户：%1）").arg(user));
}

void MainWindow::onLogout()
{
    m_stack->setCurrentWidget(m_loginPage);
    setWindowTitle(QStringLiteral("医院诊疗系统 - 用户信息管理"));
}
