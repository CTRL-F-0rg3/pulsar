#pragma once
#include <QMainWindow>
#include "markdown_editor.hpp"

class QTabWidget;
class TitleBar;
class FileTree;
class QLabel;
class QSplitter;
class QToolButton;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void closeEvent(QCloseEvent* e) override;

private slots:
    void newFile();
    void openFile();
    void openFolder();
    void saveFile();
    void saveFileAs();
    void closeTab(int idx);
    void onTabChanged(int idx);
    void onFileTreeClicked(const QString& path);
    void setModeSource();
    void setModePreview();
    void setModeSplit();
    void insertBold();
    void insertItalic();
    void insertHeading();
    void insertList();
    void insertLink();
    void insertCode();
    void insertImage();
    void insertQuote();
    void updateStatus();
    void zoomIn();
    void zoomOut();
    void zoomReset();

private:
    MarkdownEditor* currentEditor() const;
    void openInNewTab(const QString& path);
    void updateModeButtons();

    TitleBar* m_titleBar;
    FileTree* m_tree;
    QTabWidget* m_tabs;
    QLabel* m_statusPos;
    QLabel* m_statusWords;
    QLabel* m_modeLabel;
    QPushButton* m_btnSource;
    QPushButton* m_btnSplit;
    QPushButton* m_btnPreview;
};