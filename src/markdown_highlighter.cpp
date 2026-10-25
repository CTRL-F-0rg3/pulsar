#include "markdown_highlighter.hpp"

MarkdownHighlighter::MarkdownHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {

    QTextCharFormat h1;
    h1.setForeground(QColor(0xff, 0x2d, 0x95));
    h1.setFontWeight(QFont::Black);
    h1.setFontPointSize(20);
    m_rules.append({QRegularExpression("^#\\s.*$"), h1});

    QTextCharFormat h2;
    h2.setForeground(QColor(0xff, 0x5e, 0xb0));
    h2.setFontWeight(QFont::Bold);
    h2.setFontPointSize(16);
    m_rules.append({QRegularExpression("^##\\s.*$"), h2});

    QTextCharFormat h3;
    h3.setForeground(QColor(0xd0, 0x8a, 0xd4));
    h3.setFontWeight(QFont::Bold);
    h3.setFontPointSize(13);
    m_rules.append({QRegularExpression("^###\\s.*$"), h3});

    QTextCharFormat h4;
    h4.setForeground(QColor(0xa8, 0x90, 0xc4));
    h4.setFontWeight(QFont::Bold);
    m_rules.append({QRegularExpression("^####\\s.*$"), h4});

    QTextCharFormat h5;
    h5.setForeground(QColor(0xa8, 0x90, 0xc4));
    m_rules.append({QRegularExpression("^#####\\s.*$"), h5});

    QTextCharFormat h6;
    h6.setForeground(QColor(0xa8, 0x90, 0xc4));
    m_rules.append({QRegularExpression("^######\\s.*$"), h6});

    QTextCharFormat bold;
    bold.setFontWeight(QFont::Bold);
    bold.setForeground(QColor(0xfa, 0xfa, 0xfa));
    m_rules.append({QRegularExpression("\\*\\*[^*\\n]+\\*\\*"), bold});
    m_rules.append({QRegularExpression("__[^_\\n]+__"), bold});

    QTextCharFormat italic;
    italic.setFontItalic(true);
    italic.setForeground(QColor(0x94, 0xe2, 0xd5));
    m_rules.append({QRegularExpression("(?<!\\*)\\*[^*\\n]+\\*(?!\\*)"), italic});
    m_rules.append({QRegularExpression("(?<!_)_[^_\\n]+_(?!_)"), italic});

    QTextCharFormat code;
    code.setForeground(QColor(0x00, 0xff, 0x88));
    code.setFontFamilies({"Liberation Mono", "monospace"});
    code.setBackground(QColor(0x00, 0xff, 0x88, 20));
    m_rules.append({QRegularExpression("`[^`\\n]+`"), code});

    QTextCharFormat link;
    link.setForeground(QColor(0x89, 0xb4, 0xfa));
    link.setFontUnderline(true);
    m_rules.append({QRegularExpression("\\[([^\\]]+)\\]\\(([^)]+)\\)"), link});

    QTextCharFormat image;
    image.setForeground(QColor(0xfa, 0xb3, 0x87));
    image.setFontItalic(true);
    m_rules.append({QRegularExpression("!\\[[^\\]]*\\]\\([^)]+\\)"), image});

    QTextCharFormat list;
    list.setForeground(QColor(0xff, 0x2d, 0x95));
    list.setFontWeight(QFont::Bold);
    m_rules.append({QRegularExpression("^\\s*[-*+]\\s"), list});
    m_rules.append({QRegularExpression("^\\s*\\d+\\.\\s"), list});

    QTextCharFormat quote;
    quote.setForeground(QColor(0x6a, 0x6a, 0x8a));
    quote.setFontItalic(true);
    m_rules.append({QRegularExpression("^>\\s.*$"), quote});

    QTextCharFormat hr;
    hr.setForeground(QColor(0xff, 0x2d, 0x95));
    m_rules.append({QRegularExpression("^---+\\s*$"), hr});
    m_rules.append({QRegularExpression("^\\*\\*\\*+\\s*$"), hr});

    QTextCharFormat tag;
    tag.setForeground(QColor(0x00, 0xff, 0x88));
    tag.setFontWeight(QFont::Bold);
    m_rules.append({QRegularExpression("(?<![#\\w])#\\w+"), tag});

    m_codeBlockFmt.setForeground(QColor(0x00, 0xff, 0x88));
    m_codeBlockFmt.setFontFamilies({"Liberation Mono", "monospace"});
    m_codeBlockFmt.setBackground(QColor(0x00, 0xff, 0x88, 15));
    m_codeBlockStart = QRegularExpression("^```");
}

void MarkdownHighlighter::highlightBlock(const QString& text) {
    for (const auto& rule : m_rules) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    int state = previousBlockState();
    int start = 0;
    if (state != 1 && text.startsWith("```")) {
        state = 1;
        start = 0;
    }

    if (state == 1) {
        int endIdx = -1;
        if (!text.startsWith("```")) {
            endIdx = text.indexOf("```");
        }
        if (start == 0 && text.startsWith("```")) {
            endIdx = text.indexOf("```", 3);
        }

        if (endIdx >= 0) {
            setFormat(start, endIdx + 3 - start, m_codeBlockFmt);
            setCurrentBlockState(0);
        } else {
            setFormat(start, text.length() - start, m_codeBlockFmt);
            setCurrentBlockState(1);
        }
    } else {
        setCurrentBlockState(0);
    }
}