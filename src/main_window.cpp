#include "main_window.hpp"
#include "title_bar.hpp"
#include "cosmic_background.hpp"
#include "file_tree.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QMenuBar>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(1400, 950);
    setWindowTitle("Pulsar");

    auto* central = new CosmicBackground(this);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_titleBar = new TitleBar(central);
    mainLayout->addWidget(m_titleBar);

    auto* fileMenu = m_titleBar->menuBar()->addMenu("File");
    fileMenu->addAction("New File", QKeySequence::New, this, &MainWindow::newFile);
    fileMenu->addAction("Open File", QKeySequence::Open, this, &MainWindow::openFile);
    fileMenu->addAction("Open Folder", QKeySequence("Ctrl+Shift+O"), this, &MainWindow::openFolder);
    fileMenu->addAction("Save", QKeySequence::Save, this, &MainWindow::saveFile);
    fileMenu->addAction("Save As", QKeySequence::SaveAs, this, &MainWindow::saveFileAs);

    auto* viewMenu = m_titleBar->menuBar()->addMenu("View");
    viewMenu->addAction("Source", QKeySequence("Ctrl+1"), this, &MainWindow::setModeSource);
    viewMenu->addAction("Split", QKeySequence("Ctrl+2"), this, &MainWindow::setModeSplit);
    viewMenu->addAction("Preview", QKeySequence("Ctrl+3"), this, &MainWindow::setModePreview);
    viewMenu->addSeparator();
    viewMenu->addAction("Zoom In", QKeySequence::ZoomIn, this, &MainWindow::zoomIn);
    viewMenu->addAction("Zoom Out", QKeySequence::ZoomOut, this, &MainWindow::zoomOut);
    viewMenu->addAction("Reset Zoom", QKeySequence("Ctrl+0"), this, &MainWindow::zoomReset);

    auto* toolbar = new QWidget(central);
    toolbar->setStyleSheet(
        "QWidget {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop:0 rgba(30, 18, 40, 230),"
        "    stop:1 rgba(15, 8, 22, 230));"
        "  border-bottom: 1px solid rgba(255, 45, 149, 80);"
        "}"
        "QToolButton {"
        "  background: transparent; border: 1px solid transparent;"
        "  border-radius: 4px; color: #e0dde8; padding: 4px 10px;"
        "  font-weight: bold; min-width: 28px;"
        "}"
        "QToolButton:hover { border-color: rgba(255,45,149,80); background: rgba(255,45,149,15); }"
        "QPushButton {"
        "  background: rgba(0,0,0,0.3); color: #a890c4;"
        "  border: 1px solid #2a2438; border-radius: 4px;"
        "  padding: 4px 12px; font-size: 11px; font-weight: bold;"
        "}"
        "QPushButton:hover { color: #e0dde8; border-color: #ff2d95; }"
        "QPushButton:checked { background: #ff2d95; color: white; border-color: #ff2d95; }"
        "QLabel { color: #a890c4; background: transparent; }"
    );
    auto* tbLayout = new QHBoxLayout(toolbar);
    tbLayout->setContentsMargins(12, 6, 12, 6);
    tbLayout->setSpacing(4);

    auto makeTool = [&](const QString& text, const QString& tip, auto slot) {
        auto* b = new QToolButton(toolbar);
        b->setText(text);
        b->setToolTip(tip);
        b->setCursor(Qt::PointingHandCursor);
        connect(b, &QToolButton::clicked, this, slot);
        tbLayout->addWidget(b);
        return b;
    };

    makeTool("B", "Pogrubienie (Ctrl+B)", &MainWindow::insertBold);
    makeTool("I", "Kursywa (Ctrl+I)", &MainWindow::insertItalic);
    makeTool("H", "Nagłówek", &MainWindow::insertHeading);
    makeTool("•", "Lista", &MainWindow::insertList);
    makeTool("Link", "Link", &MainWindow::insertLink);
    makeTool("<>", "Kod", &MainWindow::insertCode);
    makeTool("Img", "Obraz", &MainWindow::insertImage);
    makeTool("Cyt", "Cytat", &MainWindow::insertQuote);

    tbLayout->addStretch();

    m_btnSource = new QPushButton("Source", toolbar);
    m_btnSplit = new QPushButton("Split", toolbar);
    m_btnPreview = new QPushButton("Preview", toolbar);
    m_btnSource->setCheckable(true);
    m_btnSplit->setCheckable(true);
    m_btnPreview->setCheckable(true);
    m_btnSplit->setChecked(true);
    connect(m_btnSource, &QPushButton::clicked, this, &MainWindow::setModeSource);
    connect(m_btnSplit, &QPushButton::clicked, this, &MainWindow::setModeSplit);
    connect(m_btnPreview, &QPushButton::clicked, this, &MainWindow::setModePreview);
    tbLayout->addWidget(m_btnSource);
    tbLayout->addWidget(m_btnSplit);
    tbLayout->addWidget(m_btnPreview);

    mainLayout->addWidget(toolbar);

    auto* mainSplitter = new QSplitter(Qt::Horizontal, central);
    mainSplitter->setStyleSheet(
        "QSplitter::handle { background: #2a2438; width: 2px; }"
        "QSplitter::handle:hover { background: #ff2d95; }"
    );

    m_tree = new FileTree(mainSplitter);
    m_tree->setMinimumWidth(180);
    mainSplitter->addWidget(m_tree);

    m_tabs = new QTabWidget(mainSplitter);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(true);
    m_tabs->setStyleSheet(
        "QTabWidget::pane { border: none; background: rgba(0,0,0,0.3); }"
        "QTabBar::tab {"
        "  background: rgba(0,0,0,0.5); color: #8a7a95;"
        "  padding: 8px 22px; margin-right: 2px; margin-top: 6px;"
        "  border: 1px solid rgba(255,255,255,0.1); border-bottom: none;"
        "  border-top-left-radius: 8px; border-top-right-radius: 8px;"
        "}"
        "QTabBar::tab:selected {"
        "  background: transparent; color: #ff2d95;"
        "  padding: 12px 22px 8px 22px; margin-top: 0;"
        "  border: 1px solid #ff2d95; border-bottom: none;"
        "}"
        "QTabBar::tab:hover:!selected { background: rgba(255,255,255,0.05); }"
    );
    mainSplitter->addWidget(m_tabs);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes({220, 1180});

    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(m_tree, &FileTree::markdownFileClicked, this, &MainWindow::onFileTreeClicked);

    mainLayout->addWidget(mainSplitter, 1);
    setCentralWidget(central);

    m_statusPos = new QLabel("Ln 1, Col 1", this);
    m_statusWords = new QLabel("0 słów", this);
    m_modeLabel = new QLabel("Split", this);
    m_statusPos->setStyleSheet("QLabel { color: #a890c4; padding: 4px 12px; font-size: 11px; }");
    m_statusWords->setStyleSheet("QLabel { color: #00ff88; padding: 4px 12px; font-size: 11px; }");
    m_modeLabel->setStyleSheet("QLabel { color: #ff2d95; padding: 4px 12px; font-size: 11px; font-weight: bold; }");
    statusBar()->addWidget(m_statusPos);
    statusBar()->addPermanentWidget(m_statusWords);
    statusBar()->addPermanentWidget(m_modeLabel);
    statusBar()->setStyleSheet("QStatusBar { background: rgba(10,6,16,200); border-top: 1px solid rgba(255,45,149,40); }");

    newFile();
}

MarkdownEditor* MainWindow::currentEditor() const {
    return qobject_cast<MarkdownEditor*>(m_tabs->currentWidget());
}

void MainWindow::newFile() {
    auto* ed = new MarkdownEditor(this);
    int idx = m_tabs->addTab(ed, "Nowy.md");
    m_tabs->setCurrentIndex(idx);

    connect(ed, &MarkdownEditor::modificationChanged, this, [this, ed](bool mod){
        int i = m_tabs->indexOf(ed);
        if (i < 0) return;
        QString name = ed->displayName();
        if (mod) name += " •";
        m_tabs->setTabText(i, name);
    });
    connect(ed, &MarkdownEditor::wordCountChanged, this, [this](const QString& s){
        m_statusWords->setText(s);
    });
    connect(ed, &MarkdownEditor::modeChanged, this, [this](MarkdownEditor::Mode){
        updateModeButtons();
    });

    ed->setFocus();
    m_statusWords->setText("0 słów");
    updateModeButtons();
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, "Otwórz plik",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Markdown (*.md *.markdown);;Text (*.txt);;All (*)");
    if (path.isEmpty()) return;
    openInNewTab(path);
}

void MainWindow::openFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "Otwórz folder");
    if (dir.isEmpty()) return;
    m_tree->setRootPath(dir);
}

void MainWindow::openInNewTab(const QString& path) {
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto* e = qobject_cast<MarkdownEditor*>(m_tabs->widget(i));
        if (e && e->filePath() == path) { m_tabs->setCurrentIndex(i); return; }
    }
    auto* ed = new MarkdownEditor(this);
    if (!ed->loadFile(path)) { delete ed; return; }
    int idx = m_tabs->addTab(ed, ed->displayName());
    m_tabs->setCurrentIndex(idx);

    connect(ed, &MarkdownEditor::modificationChanged, this, [this, ed](bool mod){
        int i = m_tabs->indexOf(ed);
        if (i < 0) return;
        QString name = ed->displayName();
        if (mod) name += " •";
        m_tabs->setTabText(i, name);
    });
    connect(ed, &MarkdownEditor::wordCountChanged, this, [this](const QString& s){
        m_statusWords->setText(s);
    });
    connect(ed, &MarkdownEditor::modeChanged, this, [this](MarkdownEditor::Mode){
        updateModeButtons();
    });

    m_statusWords->setText(ed->wordCount());
}

void MainWindow::saveFile() {
    auto* ed = currentEditor();
    if (!ed) return;
    if (ed->filePath().isEmpty()) { saveFileAs(); return; }
    if (!ed->saveFile())
        QMessageBox::warning(this, "Błąd", "Nie można zapisać pliku.");
    else {
        int i = m_tabs->indexOf(ed);
        if (i >= 0) m_tabs->setTabText(i, ed->displayName());
    }
}

void MainWindow::saveFileAs() {
    auto* ed = currentEditor();
    if (!ed) return;

    QString path = QFileDialog::getSaveFileName(this, "Zapisz jako",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/notatka.md",
        "Markdown (*.md);;All (*)");
    if (path.isEmpty()) return;

    if (!path.endsWith(".md", Qt::CaseInsensitive) &&
        !path.endsWith(".markdown", Qt::CaseInsensitive))
        path += ".md";

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Błąd", "Nie można zapisać pliku.");
        return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << ed->plainText();
    f.close();

    ed->setFilePath(path);

    int i = m_tabs->indexOf(ed);
    if (i >= 0) m_tabs->setTabText(i, ed->displayName());

    QMessageBox::information(this, "Zapisano", "Plik zapisany: " + path);
}

void MainWindow::closeTab(int idx) {
    auto* ed = qobject_cast<MarkdownEditor*>(m_tabs->widget(idx));
    if (ed && ed->isModified()) {
        auto r = QMessageBox::question(this, "Niezapisane zmiany",
            "Zapisać zmiany w " + ed->displayName() + "?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (r == QMessageBox::Cancel) return;
        if (r == QMessageBox::Save) { m_tabs->setCurrentIndex(idx); saveFile(); }
    }
    m_tabs->removeTab(idx);
    if (ed) ed->deleteLater();
    if (m_tabs->count() == 0) newFile();
}

void MainWindow::onTabChanged(int idx) {
    Q_UNUSED(idx);
    auto* ed = currentEditor();
    if (ed) {
        m_statusWords->setText(ed->wordCount());
        updateModeButtons();
    }
}

void MainWindow::onFileTreeClicked(const QString& path) {
    openInNewTab(path);
}

void MainWindow::setModeSource() {
    if (auto* e = currentEditor()) e->setMode(MarkdownEditor::Source);
    updateModeButtons();
}

void MainWindow::setModeSplit() {
    if (auto* e = currentEditor()) e->setMode(MarkdownEditor::Split);
    updateModeButtons();
}

void MainWindow::setModePreview() {
    if (auto* e = currentEditor()) e->setMode(MarkdownEditor::Preview);
    updateModeButtons();
}

void MainWindow::updateModeButtons() {
    auto* ed = currentEditor();
    MarkdownEditor::Mode m = ed ? ed->mode() : MarkdownEditor::Split;
    m_btnSource->setChecked(m == MarkdownEditor::Source);
    m_btnSplit->setChecked(m == MarkdownEditor::Split);
    m_btnPreview->setChecked(m == MarkdownEditor::Preview);
    m_modeLabel->setText(m == MarkdownEditor::Source ? "Source"
                       : m == MarkdownEditor::Preview ? "Preview" : "Split");
}

void MainWindow::insertBold() { if (auto* e = currentEditor()) e->insertText("**", "**"); }
void MainWindow::insertItalic() { if (auto* e = currentEditor()) e->insertText("*", "*"); }
void MainWindow::insertHeading() { if (auto* e = currentEditor()) e->insertText("# ", ""); }
void MainWindow::insertList() { if (auto* e = currentEditor()) e->insertText("- ", ""); }
void MainWindow::insertLink() { if (auto* e = currentEditor()) e->insertText("[", "](url)"); }
void MainWindow::insertCode() { if (auto* e = currentEditor()) e->insertText("`", "`"); }
void MainWindow::insertImage() { if (auto* e = currentEditor()) e->insertText("![alt](", ")"); }
void MainWindow::insertQuote() { if (auto* e = currentEditor()) e->insertText("> ", ""); }

void MainWindow::zoomIn() { if (auto* e = currentEditor()) e->zoomIn(); }
void MainWindow::zoomOut() { if (auto* e = currentEditor()) e->zoomOut(); }
void MainWindow::zoomReset() { if (auto* e = currentEditor()) e->zoomReset(); }

void MainWindow::updateStatus() {}

void MainWindow::keyPressEvent(QKeyEvent* e) {
    if (e->matches(QKeySequence::Save)) { saveFile(); return; }
    if (e->matches(QKeySequence::New)) { newFile(); return; }
    if (e->matches(QKeySequence::Open)) { openFile(); return; }
    if (e->key() == Qt::Key_B && (e->modifiers() & Qt::ControlModifier)) { insertBold(); return; }
    if (e->key() == Qt::Key_I && (e->modifiers() & Qt::ControlModifier)) { insertItalic(); return; }
    QMainWindow::keyPressEvent(e);
}

void MainWindow::closeEvent(QCloseEvent* e) {
    while (m_tabs->count() > 0) {
        auto* ed = qobject_cast<MarkdownEditor*>(m_tabs->widget(0));
        if (ed && ed->isModified()) {
            auto r = QMessageBox::question(this, "Niezapisane",
                "Zapisać " + ed->displayName() + "?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            if (r == QMessageBox::Cancel) { e->ignore(); return; }
            if (r == QMessageBox::Save) { m_tabs->setCurrentIndex(0); saveFile(); }
        }
        m_tabs->removeTab(0);
    }
    QMainWindow::closeEvent(e);
}