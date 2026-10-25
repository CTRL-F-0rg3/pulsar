#include "file_tree.hpp"
#include <QFileSystemModel>
#include <QDir>

FileTree::FileTree(QWidget* parent) : QTreeView(parent) {
    m_model = new QFileSystemModel(this);
    m_model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_model->setNameFilterDisables(false);
    m_model->setNameFilters({"*.md", "*.markdown", "*.txt"});
    setModel(m_model);
    for (int i = 1; i < 4; ++i) hideColumn(i);
    setHeaderHidden(true);
    setAnimated(false);
    setUniformRowHeights(true);
    setStyleSheet(
        "QTreeView {"
        "  background: #08050d;"
        "  color: #d0c8e0;"
        "  border: none;"
        "  border-right: 1px solid #2a2438;"
        "  padding: 8px;"
        "  font-size: 12px;"
        "}"
        "QTreeView::item { padding: 4px 0; border-radius: 4px; }"
        "QTreeView::item:hover { background: rgba(255, 45, 149, 0.1); }"
        "QTreeView::item:selected { background: #ff2d95; color: white; }"
        "QTreeView::branch { background: transparent; }"
        "QScrollBar:vertical { background: #08050d; width: 10px; }"
        "QScrollBar::handle:vertical { background: #2a2438; border-radius: 5px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background: #ff2d95; }"
    );
    connect(this, &QTreeView::clicked, this, &FileTree::onClicked);
}

void FileTree::setRootPath(const QString& path) {
    m_root = path;
    setRootIndex(m_model->setRootPath(path));
}

void FileTree::onClicked(const QModelIndex& idx) {
    if (m_model->isDir(idx)) return;
    QString path = m_model->filePath(idx);
    QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == "md" || suffix == "markdown" || suffix == "txt")
        emit markdownFileClicked(path);
}