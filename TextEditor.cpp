#include "Texteditor.h"
#include <QtWidgets>

TextEditor::TextEditor(QWidget *parent)
: QTextEdit(parent), c(0)
{
    setFontPointSize(14);
}

TextEditor::~TextEditor()
{
}

//为textEditor设置QCompleter
void TextEditor::setCompleter(QCompleter *completer)
{
    if (c)
        QObject::disconnect(c, 0, this, 0);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

//给定路径名实现插入图片
void TextEditor::insertImage(QString filename)
{
    QTextImageFormat imageFormat;
    imageFormat.setName(filename);
    textCursor().insertImage(imageFormat);
}

//融合表格中的单元格
void TextEditor::mergeCell()
{
    QTextTable *curTable=textCursor().currentTable();
    if(curTable)
    {
        curTable->mergeCells(textCursor());
    }
}

//返回所设置的QCompleter
QCompleter *TextEditor::completer() const
{
    return c;
}

//将自动补全的单词插入
void TextEditor::insertCompletion(const QString& completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

//返回光标所在的单词
QString TextEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void TextEditor::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void TextEditor::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 2
                      || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}

//设置字号的具体实现
void TextEditor::setFontSize(const QString &size)
{
    setFontPointSize(size.toFloat());
}

//设置字体加粗的具体实现
void TextEditor::setFontBold(bool checked)
{
    setFontWeight(checked?QFont::Bold:QFont::Normal);
}

//设置字体颜色的具体实现
void TextEditor::setFontColor()
{
    QColor color=QColorDialog::getColor(textColor(),this);
    if(color.isValid())
    {
        setTextColor(color);
    }
}


