#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>
#include <QTextEdit>
#include <QCompleter>

class TextEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent = 0);

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;
    ~TextEditor();
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
signals:

private slots:
    void insertCompletion(const QString &completion);
private:
    QString textUnderCursor() const;
    QCompleter *c;
};

#endif // TEXTEDITOR_H
