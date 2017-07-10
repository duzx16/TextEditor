#include <QApplication>
#include <QStringListModel>
#include "mainwindows.h"
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

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{

    //文本编辑器的底层实现
    textEdit=new TextEditor(this);
    connect(textEdit->document(),&QTextDocument::modificationChanged,this,&MainWindow::setWindowModified);

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
    setWindowTitle(tr("Text Editor [*]"));

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
}

void MainWindow::createToolBars()
{

}

void MainWindow::createFile()
{
    MainWindow *newText=new MainWindow(this->parentWidget());
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

