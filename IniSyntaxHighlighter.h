#ifndef INISYNTAXHIGHLIGHTER_H
#define INISYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QVector>

class IniSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit IniSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
};

#endif // INISYNTAXHIGHLIGHTER_H
