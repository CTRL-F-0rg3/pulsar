#pragma once
#include <QWidget>
#include <QString>

class QTextEdit;
class QTextBrowser;
class QSplitter;
class MarkdownHighlighter;
class QTimer;

class MarkdownEditor : public QWidget {
    Q_OBJECT
public:
    enum Mode { Source, Preview, Split };

    explicit MarkdownEditor(QWidget* parent = nullptr);
    ~MarkdownEditor();

    bool loadFile(const QString& path);
    bool saveFile();
    QString filePath() const { return m_path; }
    void setFilePath(const QString& path) { m_path = path; }
    QString displayName() const;
    bool isModified() const;
    void setMode(Mode m);
    Mode mode() const { return m_mode; }
    void setEditorFont(const QFont& f);
    void zoomIn();
    void zoomOut();
    void zoomReset();
    QString wordCount() const;
    QString plainText() const;

signals:
    void modificationChanged(bool modified);
    void modeChanged(Mode m);
    void wordCountChanged(const QString& text);

public slots:
    void insertText(const QString& before, const QString& after = "");

private slots:
    void onTextChanged();
    void updatePreview();

private:
    QTextEdit* m_editor;
    QTextBrowser* m_preview;
    QSplitter* m_splitter;
    MarkdownHighlighter* m_highlighter;
    QString m_path;
    Mode m_mode = Split;
    qreal m_zoom = 1.0;
    QTimer* m_updateTimer;
};