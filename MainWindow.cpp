#include <QApplication>
#include <QStringListModel>
#include "MainWindows.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{

    //文本编辑器的底层实现
    textEdit=new TextEditor(this);
    connect(textEdit->document(),&QTextDocument::modificationChanged,this,&MainWindow::setWindowModified);

    findDialog=new FindDialog(this);
    connect(findDialog->findAction(),SIGNAL(clicked(bool)),this,SLOT(takeSearch()));

    insertTableDialog=new InsertTableDialog(this);
    connect(insertTableDialog,SIGNAL(accepted()),this,SLOT(insertTable()));

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

    listLabel=new QLabel(tr("List:"));
    listBox=new QComboBox;
    listBox->addItem("Standard");
    listBox->addItem("Disc");
    listBox->addItem("Circle");
    listBox->addItem("Square");
    listBox->addItem("Decimal");
    listBox->addItem("LowerAlpha");
    listBox->addItem("UpperAlpha");
    listBox->addItem("LowerRoman");
    listBox->addItem("UpperRoman");
    connect(listBox,SIGNAL(activated(int)),textEdit,SLOT(setList(int)));
    connect(textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(changeLisBox()));
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

    findAction=new QAction(QIcon(":/icons/find"),tr("&Find"),this);
    findAction->setShortcut(QKeySequence::Find);
    findAction->setStatusTip(tr("Find and select the specific text"));
    connect(findAction,SIGNAL(triggered(bool)),this,SLOT(showFindDialog()));

    //添加格式菜单的操作
    boldAction=new QAction(QIcon(":/icons/bold"),tr("&Bold"),this);
    boldAction->setCheckable(true);
    connect(boldAction,SIGNAL(triggered(bool)),textEdit,SLOT(setFontBold(bool)));

    italicAction=new QAction(QIcon(":/icons/italic"),tr("&Italic"),this);
    italicAction->setCheckable(true);
    connect(italicAction,SIGNAL(triggered(bool)),textEdit,SLOT(setFontItalic(bool)));

    underlineAction=new QAction(QIcon(":/icons/underline"),tr("&Underline"),this);
    underlineAction->setCheckable(true);
    connect(underlineAction,SIGNAL(triggered(bool)),textEdit,SLOT(setFontUnderline(bool)));

    fontColorAction=new QAction(QIcon(":./icons/fontcolor"),tr("&Font Color"),this);
    connect(fontColorAction,SIGNAL(triggered(bool)),textEdit,SLOT(setFontColor()));

    changeFontAction(textEdit->currentCharFormat());
    connect(textEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),this,SLOT(changeFontAction(QTextCharFormat)));

    //用于表示对齐的动作
    alignGroup=new QActionGroup(this);

    leftAlignAction=new QAction(QIcon(":/icons/leftalign"),tr("&Left Align"),alignGroup);
    leftAlignAction->setCheckable(true);
    rightAlignAction=new QAction(QIcon(":/icons/rightalign"),tr("&Right Align"),alignGroup);
    rightAlignAction->setCheckable(true);
    centerAlignAction=new QAction(QIcon(":/icons/centeralign"),tr("&Center Align"),alignGroup);
    centerAlignAction->setCheckable(true);
    justifyAlignAction=new QAction(QIcon(":/icons/justifyalign"),tr("&Justify Align"),alignGroup);
    justifyAlignAction->setCheckable(true);
    connect(alignGroup,SIGNAL(triggered(QAction*)),this,SLOT(setBlockAlign(QAction*)));
    connect(textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(changeAlignAction()));
    leftAlignAction->setChecked(true);

    //TODO 把列表添加进来
    //添加插入菜单的操作
    insertImageAction=new QAction(QIcon(":/icons/insertImage"),tr("Insert &Image"),this);
    insertImageAction->setStatusTip(tr("Insert an image from file into the current cursor position"));
    connect(insertImageAction,SIGNAL(triggered(bool)),this,SLOT(insertImage()));

    insertTableAction=new QAction(QIcon(":/icons/table"),tr("Insert &Table"),this);
    insertTableAction->setShortcut(tr("Insert a table"));
    connect(insertTableAction,&QAction::triggered,insertTableDialog,&QDialog::show);
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
    editMenu->addAction(findAction);

    formatMenu=menuBar()->addMenu("&Format");
    formatMenu->addAction(boldAction);
    formatMenu->addAction(italicAction);
    formatMenu->addAction(underlineAction);
    formatMenu->addAction(fontColorAction);
    formatMenu->addSeparator();
    formatMenu->addActions(alignGroup->actions());

    insertMenu=menuBar()->addMenu("&Insert");
    insertMenu->addAction(insertImageAction);
    insertMenu->addSeparator();
    insertMenu->addAction(insertTableAction);
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
    fontBar->addAction(boldAction);
    fontBar->addAction(italicAction);
    fontBar->addAction(underlineAction);
    fontBar->addAction(fontColorAction);

    //段落格式工具栏
    blockBar=addToolBar("&Block");
    blockBar->addActions(alignGroup->actions());
    blockBar->addWidget(listBox);
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
void MainWindow::changeFontAction(const QTextCharFormat &format)
{
    fontBox->setCurrentFont(format.font());
    sizeBox->setCurrentText(QString::number(format.fontPointSize()));
    boldAction->setChecked(format.font().bold());
    italicAction->setChecked(format.fontItalic());
    underlineAction->setChecked(format.fontUnderline());
}


QString MainWindow::textUnderCursor() const
{
    QTextCursor tc = textEdit->textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

//设置段落对齐格式
void MainWindow::setBlockAlign(QAction *aim)
{
    if(aim==leftAlignAction)
        textEdit->setAlignment(Qt::AlignLeft);
    else if(aim==rightAlignAction)
        textEdit->setAlignment(Qt::AlignRight);
    else if(aim==centerAlignAction)
        textEdit->setAlignment(Qt::AlignCenter);
    else if(aim==justifyAlignAction)
        textEdit->setAlignment(Qt::AlignJustify);
}

void MainWindow::changeAlignAction()
{
   Qt::Alignment align=textEdit->alignment();
   switch(align)
   {
   case Qt::AlignLeft:leftAlignAction->setChecked(true);break;
   case Qt::AlignRight:rightAlignAction->setChecked(true);break;
   case Qt::AlignCenter:centerAlignAction->setChecked(true);break;
   case Qt::AlignJustify:justifyAlignAction->setChecked(true);break;
   }
}

//在鼠标位置变化时更新列表选项框
void MainWindow::changeLisBox()
{
    QTextCursor cursor=textEdit->textCursor();
    QTextList *curList=cursor.currentList();
    if(curList)
    {
        listBox->setCurrentIndex(-(curList->format().style()));
    }
    else
    {
        listBox->setCurrentIndex(0);
    }
}

void MainWindow::showFindDialog()
{
    findDialog->show();
}

void MainWindow::takeSearch()
{
    QString aim=findDialog->searchContent();
    bool findResult;
    QTextDocument::FindFlags options=QTextDocument::FindFlags();
    if(findDialog->fromStart()&&findDialog->isNewSearch())
        textEdit->moveCursor(QTextCursor::Start);
    if(findDialog->backward())
        options|=QTextDocument::FindBackward;
    if(findDialog->useReg())
    {
        Qt::CaseSensitivity cs=Qt::CaseInsensitive;
        if(findDialog->caseCheck())
            cs=Qt::CaseSensitive;
        QRegExp exp(aim,cs);
        findResult=textEdit->find(exp,options);
    }
    else
    {
        if(findDialog->wholeWords())
        options|=QTextDocument::FindWholeWords;
        if(findDialog->caseCheck())
            options|=QTextDocument::FindCaseSensitively;
        findResult=textEdit->find(aim,options);
    }
    if(!findResult)
    {
        QMessageBox::information(this,tr("Find result"),tr("Can't find the given text"));
        findDialog->clearLastSearch();
    }
    else
    {
        findDialog->setLastSearch();
    }
}

void MainWindow::insertImage()
{
    QString path=QFileDialog::getOpenFileName(this,tr("Open image"),".",tr("Image File(*.jpg *.png *.bmp *.jpeg *.gif)"));
    if(path.isEmpty())
    {
        QMessageBox::warning(this,tr("Paht"),tr("You didn't select any file"));
        return;
    }
    textEdit->insertImage(path);
}

void MainWindow::insertTable()
{
    int column=insertTableDialog->column(),row=insertTableDialog->row();
    textEdit->textCursor().insertTable(row,column);
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

