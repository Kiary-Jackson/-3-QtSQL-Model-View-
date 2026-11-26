#ifndef PATIENTWIDGET_H
#define PATIENTWIDGET_H

#include <QWidget>

class PatientModel;
class QTableView;
class QLineEdit;
class QLabel;
class QPushButton;
class QToolBar;
class QAction;

// 患者管理视图：工具栏 + 查询栏 + TableView(Model)，只通过 Model 访问数据
class PatientWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PatientWidget(QWidget *parent = nullptr);
    void setCurrentUser(const QString &user);

private slots:
    void onAdd();
    void onEdit();
    void onRemove();
    void onSearch();
    void onReset();
    void onRefresh();
    void onSelectionChanged();
    void onDelegateEdited(bool ok, const QString &message);

private:
    void buildUi();
    void showTip(const QString &message, bool ok = true);
    QStringList currentRowValues(int row) const;

    PatientModel *m_model;
    QTableView   *m_view;
    QLineEdit    *m_searchEdit;
    QLabel       *m_statusLabel;
    QLabel       *m_userLabel;
    QAction      *m_editAction;
    QAction      *m_removeAction;
};

#endif // PATIENTWIDGET_H
