#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>
#include <QTextEdit>

class QCompleter;

class TextEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent = 0);

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;
    ~TextEditor();
protected slots:
    //字体工具栏的具体实现
    void setFontSize(const QString &size);
    void setFontBold(bool checked);
    void setFontColor();
protected:
    //覆盖这两个时间以便于自动补全的实现
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
signals:
private slots:
    //用于实现插入自动不全的单词
    void insertCompletion(const QString &completion);
private:
    QString textUnderCursor() const;
    QCompleter *c;
};

#endif // TEXTEDITOR_H
