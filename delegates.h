#ifndef DELEGATES_H
#define DELEGATES_H

#include <QStyledItemDelegate>
#include <QStringList>

// 自定义代理：下拉框编辑（用于性别、状态列），提交时直接落库并回读
class ComboDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ComboDelegate(const QStringList &items, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

signals:
    void editCommitted(bool ok, const QString &message);

private:
    QStringList m_items;
};

#endif // DELEGATES_H
