#include <QApplication>
#include <QStringListModel>
#include "MainWindows.h"
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QMainWindow>
#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QTextDocument>
#include <QString>
#include <QKeySequence>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QIODevice>
#include <QTextStream>
#include <QEvent>
#include <QWidget>
#include <QCloseEvent>
#include <QCompleter>
#include <QMenuBar>
#include <QToolBar>
#include <QDebug>
#include <QLabel>
#include <QToolButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QFontDatabase>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{

    //文本编辑器的底层实现
    textEdit=new TextEditor(this);
    connect(textEdit->document(),&QTextDocument::modificationChanged,this,&MainWindow::setWindowModified);

    createButtons();
    createActions();
    createMenus();
    createToolBars();

    statusBar();


    completer=new QCompleter(this);
    completer->setModel(modelFromFile(":/text/wordlist"));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    textEdit->setCompleter(completer);
    setCentralWidget(textEdit);
    resize(800, 600);
    setWindowTitle(tr("Text Editor[*]"));

}
QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}





MainWindow::~MainWindow()
{

}

void MainWindow::createButtons()
{
    fontLabel=new QLabel(tr("Font:"));
    fontBox=new QFontComboBox;
    fontBox->setFontFilters(QFontComboBox::ScalableFonts);
    connect(fontBox,SIGNAL(activated(QString)),textEdit,SLOT(setFontFamily(QString)));

    sizeLabel=new QLabel(tr("Size:"));
    sizeBox=new QComboBox;
    sizeBox->setEditable(true);
    QFontDatabase db;
    foreach (int size, db.standardSizes()) {
        sizeBox->addItem(QString::number(size));
    }
    connect(sizeBox,SIGNAL(activated(QString)),textEdit,SLOT(setFontSize(QString)));

    boldButton=new QToolButton;
    boldButton->setIcon(QIcon(":/icons/bold"));
    boldButton->setCheckable(true);
    connect(boldButton,SIGNAL(clicked(bool)),textEdit,SLOT(setFontBold(bool)));

    italicButton=new QToolButton;
    italicButton->setIcon(QIcon(":/icons/italic"));
    italicButton->setCheckable(true);
    connect(italicButton,SIGNAL(clicked(bool)),textEdit,SLOT(setFontItalic(bool)));

    underlineButton=new QToolButton;
    underlineButton->setIcon(QIcon(":/icons/underline"));
    underlineButton->setCheckable(true);
    connect(underlineButton,SIGNAL(clicked(bool)),textEdit,SLOT(setFontUnderline(bool)));

    changeFontToolBar(textEdit->currentCharFormat());
    connect(textEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),this,SLOT(changeFontToolBar(QTextCharFormat)));

}

void MainWindow::createActions()
{
    //添加文件菜单的操作
    openFileAction=new QAction(QIcon(":/icons/open_file"),tr("&Open..."),this);
    openFileAction->setShortcut(QKeySequence::Open);
    openFileAction->setStatusTip(tr("Open an existing file"));
    connect(openFileAction,&QAction::triggered,this,&MainWindow::openFile);

    saveFileAction=new QAction(QIcon(":/icons/save_file"),tr("&Save"),this);
    saveFileAction->setShortcut(QKeySequence::Save);
    saveFileAction->setStatusTip(tr("Save the current file"));
    connect(saveFileAction,&QAction::triggered,this,&MainWindow::saveFile);

    createFileAction=new QAction(QIcon(":/icons/create_file"),tr("&New"),this);
    createFileAction->setShortcut(QKeySequence::New);
    createFileAction->setStatusTip(tr("Create a new file"));
    connect(createFileAction,&QAction::triggered,this,&MainWindow::createFile);

    saveAsFileAction=new QAction(QIcon(":/icons/saveAs"),tr("&Save As..."),this);
    saveAsFileAction->setShortcut(QKeySequence::SaveAs);
    saveAsFileAction->setStatusTip(tr("Save the current file as a new file"));
    connect(saveAsFileAction,&QAction::triggered,this,&MainWindow::saveAsFile);

    //添加编辑菜单的操作
    undoAction=new QAction(QIcon(":/icons/undo"),tr("&Undo"),this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo the last operation"));
    connect(undoAction,&QAction::triggered,textEdit,&QTextEdit::undo );
    undoAction->setEnabled(false);
    connect(textEdit,&QTextEdit::undoAvailable,undoAction,&QAction::setEnabled);

    redoAction=new QAction(QIcon(":/icons/redo"),tr("&Redo"),this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setStatusTip(tr("Redo the last operation"));
    connect(redoAction,&QAction::triggered,textEdit,&QTextEdit::redo );
    redoAction->setEnabled(false);
    connect(textEdit,&QTextEdit::redoAvailable,redoAction,&QAction::setEnabled);

    cutAction=new QAction(QIcon(":/icons/cut"),tr("&Cut"),this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Copies the selected text to the clipboard and deletes it from the edit."));
    connect(cutAction,&QAction::triggered,textEdit,&QTextEdit::cut );
    cutAction->setEnabled(false);
    connect(textEdit,&QTextEdit::copyAvailable,cutAction,&QAction::setEnabled);

    copyAction=new QAction(QIcon(":/icons/copy"),tr("&Copy"),this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copies the selected text to the clipboard."));
    connect(copyAction,&QAction::triggered,textEdit,&QTextEdit::copy );
    copyAction->setEnabled(false);
    connect(textEdit,&QTextEdit::copyAvailable,copyAction,&QAction::setEnabled);

    pasteAction=new QAction(QIcon(":/icons/paste"),tr("&Paste"),this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Pastes the text from the clipboard into the text edit at the current cursor position."));
    connect(pasteAction,&QAction::triggered,textEdit,&QTextEdit::paste);

    selectallAction=new QAction(QIcon(":/icons/selectall"),tr("&Select all"),this);
    selectallAction->setShortcut(QKeySequence::SelectAll);
    selectallAction->setStatusTip(tr("Select the whole document"));
    connect(selectallAction,&QAction::triggered,textEdit,&QTextEdit::selectAll);



}

void MainWindow::createMenus()
{
    fileMenu=menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(createFileAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(saveAsFileAction);

    editMenu=menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(selectallAction);
}

void MainWindow::createToolBars()
{
    //文件工具栏
    fileBar=addToolBar("&File");
    fileBar->addAction(createFileAction);
    fileBar->addAction(openFileAction);
    fileBar->addAction(saveFileAction);

    //编辑工具栏
    editBar=addToolBar("&Edit");
    editBar->addAction(undoAction);
    editBar->addAction(redoAction);
    editBar->addAction(cutAction);
    editBar->addAction(copyAction);
    editBar->addAction(pasteAction);

    //字体工具栏
    fontBar=addToolBar("&Font");
    fontBar->addWidget(fontLabel);
    fontBar->addWidget(fontBox);
    fontBar->addWidget(sizeLabel);
    fontBar->addWidget(sizeBox);
    fontBar->addWidget(boldButton);
    fontBar->addWidget(italicButton);
    fontBar->addWidget(underlineButton);
}

void MainWindow::createFile()
{
    MainWindow *newText=new MainWindow(this->parentWidget());
    newText->setAttribute(Qt::WA_DeleteOnClose);
    newText->show();
}

void MainWindow::openFile()
{
    QString path=QFileDialog::getOpenFileName(this,tr("Open File"),".",tr("Text Files(*.txt);;HTML Files(*.html)"));
    if(!path.isEmpty())
    {
        if(!curFileName.isEmpty()||!textEdit->document()->isEmpty())
        {
            MainWindow *newText=new MainWindow(this->parentWidget());
            newText->setAttribute(Qt::WA_DeleteOnClose);
            newText->show();
            newText->loadFile(path);

        }
        else
            loadFile(path);
    }
    else
    {
        QMessageBox::warning(this,tr("Path"),tr("You did not select any file."));
    }
}

void MainWindow::saveFile()
{
    if(curFileName.isEmpty())
    {
        QString path=QFileDialog::getSaveFileName(this,tr("Save File"),".",tr("Text File(*.txt);;HTML File(*.html)"));
        if(!path.isEmpty())
            serialFile(path);
        else
            QMessageBox::warning(this,tr("Path"),tr("You did not select any file."));
    }
    else
        serialFile(curFileName);
}

void MainWindow::saveAsFile()
{
    QString path=QFileDialog::getSaveFileName(this,tr("Save File"),".",tr("Text File(*.txt);;HTML File(*.html)"));
    if(!path.isEmpty())
        serialFile(path);
    else
        QMessageBox::warning(this,tr("Path"),tr("You did not select any file."));

}

void MainWindow::loadFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("Open File"),tr("Cannot open file:\n%1").arg(curFileName));
        return;
    }
    QTextStream in(&file);
    if(filename.endsWith(".html"))
        textEdit->setHtml(in.readAll());
    else
        textEdit->setText(in.readAll());
    curFileName=filename;
    file.close();
}




void MainWindow::serialFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,tr("Save File"),tr("Cannot save file:\n%1").arg(curFileName));
        return;
    }
    QTextStream out(&file);
    if(filename.endsWith(".html"))
        out<<textEdit->toHtml();
    else
        out<<textEdit->toPlainText();
    file.close();
    curFileName=filename;
    setWindowModified(false);
}

//用于在光标所在处的格式发生变化时更改字体工具栏的函数
void MainWindow::changeFontToolBar(const QTextCharFormat &format)
{
    fontBox->setCurrentFont(format.font());
    qDebug()<<format.fontPointSize();
    sizeBox->setCurrentText(QString::number(format.fontPointSize()));
    boldButton->setChecked(format.font().bold());
    italicButton->setChecked(format.fontItalic());
    underlineButton->setChecked(format.fontUnderline());
}


QString MainWindow::textUnderCursor() const
{
    QTextCursor tc = textEdit->textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

//覆盖事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isWindowModified())
    {
        QMessageBox::StandardButton result=QMessageBox::question(this,tr("Quit"),tr("You have unsaved changes,are you sure to quit?"),QMessageBox::Yes|QMessageBox::Cancel|QMessageBox::Save,QMessageBox::Save);
        switch(result)
        {
            case QMessageBox::Save:
                saveFile();
            case QMessageBox::Yes:
                event->accept();
                break;
            default:
                event->ignore();

        }
    }
    else
        event->accept();

}

