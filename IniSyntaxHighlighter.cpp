#include "IniSyntaxHighlighter.h"
#include <QColor>
#include <QFont>

IniSyntaxHighlighter::IniSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Section headers.
    QTextCharFormat sectionFormat;
    sectionFormat.setForeground(QColor(255, 59, 59));
    sectionFormat.setFontWeight(QFont::Bold);
    highlightingRules.append({QRegularExpression(R"(\[.*\])"), sectionFormat});

    // Keys.
    QTextCharFormat keyFormat;
    keyFormat.setForeground(QColor(48, 139, 252));
    highlightingRules.append({QRegularExpression(R"(^\s*[^=]+(?==))"), keyFormat});

    // Values.
    QTextCharFormat valueFormat;
    valueFormat.setForeground(Qt::black);
    highlightingRules.append({QRegularExpression(R"(=(.*))"), valueFormat});

    // Comments.
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);
    highlightingRules.append({QRegularExpression(R"(;.*$)"), commentFormat});
}

void IniSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const auto &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
