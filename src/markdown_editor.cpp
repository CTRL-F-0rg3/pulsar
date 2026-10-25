#include "markdown_editor.hpp"
#include "markdown_highlighter.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextEdit>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>

MarkdownEditor::MarkdownEditor(QWidget* parent) : QWidget(parent) {
    m_splitter = new QSplitter(Qt::Horizontal, this);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_splitter);

    m_editor = new QTextEdit(this);
    m_editor->setAcceptRichText(false);
    m_editor->setFont(QFont("JetBrains Mono", 11));
    m_editor->setStyleSheet(
        "QTextEdit {"
        "  background: #0a0610;"
        "  color: #e0dde8;"
        "  border: none;"
        "  padding: 20px;"
        "  selection-background-color: #ff2d95;"
        "  selection-color: white;"
        "}"
        "QScrollBar:vertical { background: #0a0610; width: 12px; }"
        "QScrollBar::handle:vertical { background: #2a2438; border-radius: 6px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background: #ff2d95; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar:horizontal { background: #0a0610; height: 12px; }"
        "QScrollBar::handle:horizontal { background: #2a2438; border-radius: 6px; min-width: 30px; }"
        "QScrollBar::handle:horizontal:hover { background: #ff2d95; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }"
    );
    m_highlighter = new MarkdownHighlighter(m_editor->document());

    m_preview = new QTextBrowser(this);
    m_preview->setOpenExternalLinks(true);
    m_preview->setStyleSheet(
        "QTextBrowser {"
        "  background: #080510;"
        "  color: #e0dde8;"
        "  border: none;"
        "  border-left: 1px solid #2a2438;"
        "  padding: 24px;"
        "}"
    );

    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_preview);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setSizes({500, 500});

    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(300);
    connect(m_updateTimer, &QTimer::timeout, this, &MarkdownEditor::updatePreview);

    connect(m_editor, &QTextEdit::textChanged, this, &MarkdownEditor::onTextChanged);

    updatePreview();
}

MarkdownEditor::~MarkdownEditor() = default;

bool MarkdownEditor::loadFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);
    m_editor->setPlainText(in.readAll());
    f.close();
    m_path = path;
    m_editor->document()->setModified(false);
    updatePreview();
    return true;
}

bool MarkdownEditor::saveFile() {
    if (m_path.isEmpty()) return false;
    QFile f(m_path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << m_editor->toPlainText();
    f.close();
    m_editor->document()->setModified(false);
    emit modificationChanged(false);
    return true;
}

QString MarkdownEditor::plainText() const {
    return m_editor->toPlainText();
}

QString MarkdownEditor::displayName() const {
    if (m_path.isEmpty()) return "Untitled";
    return QFileInfo(m_path).fileName();
}

bool MarkdownEditor::isModified() const {
    return m_editor->document()->isModified();
}

void MarkdownEditor::setMode(Mode m) {
    m_mode = m;
    if (m == Source) {
        m_preview->setVisible(false);
        m_editor->setVisible(true);
    } else if (m == Preview) {
        m_editor->setVisible(false);
        m_preview->setVisible(true);
    } else {
        m_editor->setVisible(true);
        m_preview->setVisible(true);
        m_splitter->setSizes({500, 500});
    }
    emit modeChanged(m);
}

void MarkdownEditor::setEditorFont(const QFont& f) {
    m_editor->setFont(f);
}

void MarkdownEditor::zoomIn() {
    m_zoom = qMin(m_zoom * 1.15, 3.0);
    QFont f = m_editor->font();
    f.setPointSizeF(11 * m_zoom);
    m_editor->setFont(f);
}

void MarkdownEditor::zoomOut() {
    m_zoom = qMax(m_zoom / 1.15, 0.5);
    QFont f = m_editor->font();
    f.setPointSizeF(11 * m_zoom);
    m_editor->setFont(f);
}

void MarkdownEditor::zoomReset() {
    m_zoom = 1.0;
    QFont f = m_editor->font();
    f.setPointSizeF(11);
    m_editor->setFont(f);
}

QString MarkdownEditor::wordCount() const {
    QString text = m_editor->toPlainText().trimmed();
    if (text.isEmpty()) return "0 słów";
    int words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).size();
    int chars = text.length();
    return QString("%1 słów · %2 znaków").arg(words).arg(chars);
}

void MarkdownEditor::onTextChanged() {
    m_updateTimer->start();
    emit modificationChanged(isModified());
    emit wordCountChanged(wordCount());
}

void MarkdownEditor::updatePreview() {
    QString markdown = m_editor->toPlainText();

    m_preview->document()->setDefaultStyleSheet(
        "h1 { color: #ff2d95; font-size: 24pt; font-weight: 900; border-bottom: 2px solid #ff2d95; }"
        "h2 { color: #ff5eb0; font-size: 18pt; font-weight: bold; border-bottom: 1px solid #ff5eb0; }"
        "h3 { color: #d08ad4; font-size: 15pt; font-weight: bold; }"
        "h4, h5, h6 { color: #a890c4; font-weight: bold; }"
        "p { color: #e0dde8; line-height: 1.5; }"
        "a { color: #89b4fa; text-decoration: underline; }"
        "code { color: #00ff88; font-family: 'Liberation Mono', monospace; background: rgba(0,255,136,0.1); padding: 2px 4px; }"
        "pre { background: #150a14; padding: 12px; border-left: 3px solid #ff2d95; }"
        "blockquote { color: #a890c4; font-style: italic; border-left: 3px solid #ff2d95; padding-left: 12px; margin-left: 0; }"
        "ul, ol { color: #e0dde8; }"
        "li { margin-bottom: 4px; }"
        "hr { border: none; border-top: 2px solid #ff2d95; margin: 16px 0; }"
        "table { border-collapse: collapse; }"
        "td, th { border: 1px solid #2a2438; padding: 6px; }"
        "th { background: #150a14; color: #ff2d95; font-weight: bold; }"
    );

    m_preview->document()->setMarkdown(markdown, QTextDocument::MarkdownDialectGitHub);
    m_preview->document()->setModified(false);
}

void MarkdownEditor::insertText(const QString& before, const QString& after) {
    QTextCursor c = m_editor->textCursor();
    QString sel = c.selectedText();
    if (c.hasSelection()) {
        c.insertText(before + sel + after);
    } else {
        c.insertText(before + after);
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, after.length());
        m_editor->setTextCursor(c);
    }
    m_editor->setFocus();
}