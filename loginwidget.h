#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

class QLineEdit;

// 登录视图：校验账号密码（区分正确与错误凭据），成功后发信号
class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);

signals:
    void loginSucceeded(const QString &username);

private slots:
    void tryLogin();

private:
    void buildUi();

    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
};

#endif // LOGINWIDGET_H
