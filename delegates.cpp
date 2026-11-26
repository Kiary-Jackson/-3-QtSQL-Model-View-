#include "delegates.h"

#include <QApplication>
#include <QComboBox>
#include <QSqlError>
#include <QSqlTableModel>

ComboDelegate::ComboDelegate(const QStringList &items, QObject *parent)
    : QStyledItemDelegate(parent), m_items(items)
{
}

QWidget *ComboDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QComboBox *editor = new QComboBox(parent);
    editor->addItems(m_items);
    editor->setFrame(false);
    return editor;
}

void ComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!combo) return;
    const QString current = index.data(Qt::EditRole).toString();
    const int pos = combo->findText(current);
    if (pos >= 0)
        combo->setCurrentIndex(pos);
}

void ComboDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!combo || !model) return;

    const QString value = combo->currentText();
    QSqlTableModel *sqlModel = qobject_cast<QSqlTableModel *>(model);
    const bool ok = model->setData(index, value, Qt::EditRole);
    const bool submitted = sqlModel ? sqlModel->submitAll() : false;
    if (ok && submitted) {
        emit const_cast<ComboDelegate *>(this)->editCommitted(true, QString());
    } else {
        const QString msg = sqlModel ? sqlModel->lastError().text()
                                     : QStringLiteral("当前模型不支持提交。");
        if (sqlModel) sqlModel->revertAll();
        emit const_cast<ComboDelegate *>(this)->editCommitted(false, msg);
    }
}

void ComboDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
