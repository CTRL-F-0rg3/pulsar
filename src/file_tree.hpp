#pragma once
#include <QTreeView>
#include <QFileSystemModel>

class FileTree : public QTreeView {
    Q_OBJECT
public:
    explicit FileTree(QWidget* parent = nullptr);
    void setRootPath(const QString& path);
    QString rootPath() const { return m_root; }

signals:
    void markdownFileClicked(const QString& path);

private slots:
    void onClicked(const QModelIndex& idx);

private:
    QFileSystemModel* m_model;
    QString m_root;
};