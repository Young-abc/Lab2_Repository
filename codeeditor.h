#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QObject>
#include <QPlainTextEdit>
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    bool isLineNumberVisible();
    void setLineNumberVisible(bool visible);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    bool m_lineNumberVisible = true;
};

// 行号显示区域（作为QPlainTextEdit的子控件）
class LineNumberArea : public QWidget
{
    Q_OBJECT
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    // 计算行号区域宽度（根据最大行号位数动态调整）
    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    // 绘制行号
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;  // 关联的文本编辑器
};



#endif // CODEEDITOR_H
