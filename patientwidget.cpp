#include "patientwidget.h"
#include "patientmodel.h"
#include "patientdialog.h"
#include "delegates.h"
#include "dbmanager.h"
#include "config.h"

#include <QAbstractItemView>
#include <QAction>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlTableModel>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>

PatientWidget::PatientWidget(QWidget *parent)
    : QWidget(parent), m_model(nullptr), m_view(nullptr),
      m_searchEdit(nullptr), m_statusLabel(nullptr), m_userLabel(nullptr),
      m_editAction(nullptr), m_removeAction(nullptr)
{
    m_model = new PatientModel(this);
    m_model->reload();
    buildUi();
}

void PatientWidget::setCurrentUser(const QString &user)
{
    if (m_userLabel)
        m_userLabel->setText(QStringLiteral("当前用户：%1").arg(user));
}

void PatientWidget::buildUi()
{
    // ---- 工具栏（完全用代码添加，不依赖 .ui 文件）----
    QToolBar *bar = new QToolBar(QStringLiteral("操作"), this);
    bar->setMovable(false);
    bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QAction *addAction = bar->addAction(QStringLiteral("新增"));
    m_editAction      = bar->addAction(QStringLiteral("修改"));
    m_removeAction    = bar->addAction(QStringLiteral("删除"));
    bar->addSeparator();
    QAction *refreshAction = bar->addAction(QStringLiteral("刷新"));

    connect(addAction,      &QAction::triggered, this, &PatientWidget::onAdd);
    connect(m_editAction,   &QAction::triggered, this, &PatientWidget::onEdit);
    connect(m_removeAction, &QAction::triggered, this, &PatientWidget::onRemove);
    connect(refreshAction,  &QAction::triggered, this, &PatientWidget::onRefresh);

    // ---- 查询栏 ----
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(QStringLiteral("输入姓名 / 电话 / 科室 / 状态 / 备注 查询"));
    m_searchEdit->setClearButtonEnabled(true);

    QPushButton *searchBtn = new QPushButton(QStringLiteral("查询"), this);
    QPushButton *resetBtn  = new QPushButton(QStringLiteral("重置"), this);
    connect(searchBtn, &QPushButton::clicked, this, &PatientWidget::onSearch);
    connect(resetBtn,  &QPushButton::clicked, this, &PatientWidget::onReset);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &PatientWidget::onSearch);

    QHBoxLayout *searchRow = new QHBoxLayout;
    searchRow->addWidget(new QLabel(QStringLiteral("快速查询："), this));
    searchRow->addWidget(m_searchEdit, 1);
    searchRow->addWidget(searchBtn);
    searchRow->addWidget(resetBtn);

    // ---- 视图 ----
    m_view = new QTableView(this);
    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    m_view->setAlternatingRowColors(true);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->setColumnWidth(PC_Name, 110);
    m_view->setColumnWidth(PC_Phone, 130);
    m_view->setColumnWidth(PC_VisitDate, 110);
    m_view->setColumnWidth(PC_Status, 90);

    // 自定义代理：性别 / 状态列用下拉编辑
    ComboDelegate *genderDelegate = new ComboDelegate(
        {QStringLiteral("男"), QStringLiteral("女"), QStringLiteral("其他")}, this);
    ComboDelegate *statusDelegate = new ComboDelegate(
        {QStringLiteral("待诊"), QStringLiteral("就诊中"),
         QStringLiteral("已完成"), QStringLiteral("已取消")}, this);
    m_view->setItemDelegateForColumn(PC_Gender, genderDelegate);
    m_view->setItemDelegateForColumn(PC_Status, statusDelegate);
    connect(genderDelegate, &ComboDelegate::editCommitted, this, &PatientWidget::onDelegateEdited);
    connect(statusDelegate, &ComboDelegate::editCommitted, this, &PatientWidget::onDelegateEdited);

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PatientWidget::onSelectionChanged);
    connect(m_model, &QAbstractItemModel::modelReset, this, &PatientWidget::onSelectionChanged);

    // ---- 状态栏 ----
    m_statusLabel = new QLabel(this);
    m_userLabel = new QLabel(this);
    QLabel *devLabel = new QLabel(QStringLiteral("开发者：%1    学号：%2")
                                      .arg(AppConfig::DeveloperName, AppConfig::DeveloperStudentId), this);
    devLabel->setStyleSheet(QStringLiteral("color:#444;"));

    QHBoxLayout *statusRow = new QHBoxLayout;
    statusRow->addWidget(m_statusLabel);
    statusRow->addStretch(1);
    statusRow->addWidget(m_userLabel);
    statusRow->addSpacing(24);
    statusRow->addWidget(devLabel);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->addWidget(bar);
    root->addLayout(searchRow);
    root->addWidget(m_view, 1);
    root->addLayout(statusRow);

    onSelectionChanged();
    showTip(QStringLiteral("就绪"));
}

void PatientWidget::showTip(const QString &message, bool ok)
{
    if (!m_statusLabel) return;
    m_statusLabel->setText(QStringLiteral("记录数：%1  |  %2")
                               .arg(m_model ? m_model->rowCount() : 0)
                               .arg(message));
    m_statusLabel->setStyleSheet(ok ? QStringLiteral("color:#2b7a2b;")
                                    : QStringLiteral("color:#c0392b;"));
}

void PatientWidget::onSelectionChanged()
{
    const bool hasSelection = m_view && m_view->currentIndex().isValid();
    if (m_editAction)   m_editAction->setEnabled(hasSelection);
    if (m_removeAction) m_removeAction->setEnabled(hasSelection);
}

QStringList PatientWidget::currentRowValues(int row) const
{
    QStringList v;
    for (int col = PC_Name; col <= PC_Note; ++col)
        v << m_model->data(m_model->index(row, col), Qt::EditRole).toString();
    return v;
}

void PatientWidget::onAdd()
{
    PatientDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("新增患者"));
    if (dlg.exec() != QDialog::Accepted)
        return;

    QString error;
    if (!m_model->addPatient(dlg.values(), &error)) {
        QMessageBox::critical(this, QStringLiteral("新增失败"), error);
        showTip(QStringLiteral("新增失败"), false);
        return;
    }
    showTip(QStringLiteral("新增成功"));
}

void PatientWidget::onEdit()
{
    const int row = m_view->currentIndex().row();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("请先在列表中选择一条记录。"));
        return;
    }

    PatientDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("修改患者"));
    dlg.setValues(currentRowValues(row));
    if (dlg.exec() != QDialog::Accepted)
        return;

    QString error;
    if (!m_model->updatePatient(row, dlg.values(), &error)) {
        QMessageBox::critical(this, QStringLiteral("修改失败"), error);
        showTip(QStringLiteral("修改失败"), false);
        return;
    }
    showTip(QStringLiteral("修改成功，视图已刷新"));
}

void PatientWidget::onRemove()
{
    const int row = m_view->currentIndex().row();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("请先在列表中选择一条记录。"));
        return;
    }
    const int id = m_model->recordId(row);
    const QString name = m_model->displayName(row);

    // 删除确认
    const QMessageBox::StandardButton btn = QMessageBox::question(
        this, QStringLiteral("删除确认"),
        QStringLiteral("确定要删除患者【%1】（编号 %2）吗？\n此操作不可撤销。")
            .arg(name, QString::number(id)),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (btn != QMessageBox::Yes)
        return;

    QString error;
    if (!m_model->removePatient(row, &error)) {
        QMessageBox::critical(this, QStringLiteral("删除失败"), error);
        showTip(QStringLiteral("删除失败"), false);
        return;
    }
    showTip(QStringLiteral("已删除，视图已刷新"));
}

void PatientWidget::onSearch()
{
    QString error;
    if (!m_model->search(m_searchEdit->text(), &error)) {
        QMessageBox::critical(this, QStringLiteral("查询异常"), error);
        showTip(QStringLiteral("查询异常"), false);
        return;
    }
    if (m_model->rowCount() == 0)
        showTip(QStringLiteral("查询无结果（没有匹配的患者）"), false);
    else
        showTip(QStringLiteral("查询完成"));
}

void PatientWidget::onReset()
{
    m_searchEdit->clear();
    QString error;
    if (!m_model->search(QString(), &error)) {
        QMessageBox::critical(this, QStringLiteral("刷新失败"), error);
        showTip(QStringLiteral("刷新失败"), false);
        return;
    }
    showTip(QStringLiteral("已显示全部记录"));
}

void PatientWidget::onRefresh()
{
    QString error;
    if (!m_model->reload(&error)) {
        QMessageBox::critical(this, QStringLiteral("刷新失败"), error);
        showTip(QStringLiteral("刷新失败"), false);
        return;
    }
    showTip(QStringLiteral("已从数据库重新加载"));
}

void PatientWidget::onDelegateEdited(bool ok, const QString &message)
{
    if (ok) {
        showTip(QStringLiteral("单元格修改已保存到数据库"));
    } else {
        QMessageBox::critical(this, QStringLiteral("修改失败"),
                              QStringLiteral("代理编辑提交失败：%1").arg(message));
        showTip(QStringLiteral("修改失败"), false);
        m_model->reload();
    }
}
