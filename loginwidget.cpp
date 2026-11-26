#include "loginwidget.h"
#include "dbmanager.h"
#include "config.h"
#include "patientwidget.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QVBoxLayout>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(QStringLiteral("用户登录"));
    buildUi();
}

void LoginWidget::buildUi()
{
    m_userEdit = new QLineEdit(this);
    m_userEdit->setPlaceholderText(QStringLiteral("请输入用户名"));
    m_userEdit->setText(AppConfig::DefaultUser);

    m_passEdit = new QLineEdit(this);
    m_passEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setText(AppConfig::DefaultPassword);

    QPushButton *loginBtn = new QPushButton(QStringLiteral("登 录"), this);
    loginBtn->setFixedHeight(40);
    loginBtn->setStyleSheet(QStringLiteral(
        "QPushButton{background:#2f6fed;color:white;border-radius:5px;font-weight:bold;}"
        "QPushButton:hover{background:#1f5ed8;}"));

    QLabel *title = new QLabel(QStringLiteral("医院诊疗系统 - 用户信息管理"), this);
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setPointSize(16);
    f.setBold(true);
    title->setFont(f);

    QLabel *dev = new QLabel(QStringLiteral("开发者：%1    学号：%2")
                                 .arg(AppConfig::DeveloperName, AppConfig::DeveloperStudentId), this);
    dev->setAlignment(Qt::AlignCenter);
    dev->setStyleSheet(QStringLiteral("color:#555;"));

    QLabel *hint = new QLabel(QStringLiteral("默认账号：admin / 123456（可在 config.h 中修改）"), this);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet(QStringLiteral("color:#888;font-size:11px;"));

    QFormLayout *form = new QFormLayout;
    form->addRow(QStringLiteral("用户名"), m_userEdit);
    form->addRow(QStringLiteral("密码"), m_passEdit);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->addStretch(1);
    root->addWidget(title);
    root->addSpacing(6);
    root->addWidget(dev);
    root->addStretch(1);
    root->addLayout(form);
    root->addSpacing(12);
    root->addWidget(loginBtn);
    root->addSpacing(6);
    root->addWidget(hint);
    root->addStretch(2);

    connect(loginBtn, &QPushButton::clicked, this, &LoginWidget::tryLogin);
    connect(m_passEdit, &QLineEdit::returnPressed, this, &LoginWidget::tryLogin);
}

void LoginWidget::tryLogin()
{
    const QString user = m_userEdit->text().trimmed();
    const QString pass = m_passEdit->text();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("登录失败"),
                             QStringLiteral("用户名和密码均不能为空。"));
        return;
    }

    if (!DBManager::instance()->isOpen()) {
        QMessageBox::critical(this, QStringLiteral("登录失败"),
                              QStringLiteral("数据库未连接：%1")
                                  .arg(DBManager::instance()->lastError()));
        return;
    }

    // 参数化查询，区分"凭据错误"与"SQL 异常"
    QSqlQuery q;
    QString error;
    const bool ok = DBManager::instance()->exec(
        QStringLiteral("SELECT password FROM users WHERE username=?"),
        QVariantList() << user, &q, &error);
    if (!ok) {
        QMessageBox::critical(this, QStringLiteral("查询异常"), error);
        return;
    }

    if (!q.next()) {
        QMessageBox::warning(this, QStringLiteral("登录失败"),
                             QStringLiteral("用户名不存在，请检查后重试。"));
        m_passEdit->clear();
        m_passEdit->setFocus();
        return;
    }
    if (q.value(0).toString() != pass) {
        QMessageBox::warning(this, QStringLiteral("登录失败"),
                             QStringLiteral("密码错误，请重新输入。"));
        m_passEdit->selectAll();
        m_passEdit->setFocus();
        return;
    }

    emit loginSucceeded(user);
}
