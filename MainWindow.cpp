#include <QApplication>
#include <QStringListModel>
#include "MainWindows.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{

    //创建文本编辑器
    textEdit=new TextEditor(this);
    connect(textEdit->document(),&QTextDocument::modificationChanged,this,&MainWindow::setWindowModified);

    //创建查找对话框
    findDialog=new FindDialog(this);
    connect(findDialog,SIGNAL(find()),this,SLOT(takeSearch()));

    //创建插入表格对话框
    insertTableDialog=new InsertTableDialog(this);
    connect(insertTableDialog,SIGNAL(accepted()),this,SLOT(insertTable()));

    //创建调整图片大小对话框
    modifyImageDialog=new ModifyImageDialog(this);
    connect(modifyImageDialog,SIGNAL(accepted()),this,SLOT(setImageSize()));

    //创建GUI中的元素
    createButtons();
    createActions();
    createMenus();
    createToolBars();

    //显示状态栏
    statusBar();

    //设置自动补全功能
    completer=new QCompleter(this);
    completer->setModel(modelFromFile(":/text/wordlist"));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    textEdit->setCompleter(completer);

    //设置窗口布局
    setCentralWidget(textEdit);
    resize(800, 600);
    setWindowTitle(tr("Text Editor[*]"));
    loadSettings();

}

//用于读取自动补全的wordlist
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

//创建工具栏上按钮和选项框
void MainWindow::createButtons()
{
    //创建字体选项框
    fontLabel=new QLabel(tr("Font:"));
    fontBox=new QFontComboBox;
    fontBox->setFontFilters(QFontComboBox::ScalableFonts);
    connect(fontBox,SIGNAL(activated(QString)),textEdit,SLOT(setFontFamily(QString)));

    //创建字号选项框
    sizeLabel=new QLabel(tr("Size:"));
    sizeBox=new QComboBox;
    sizeBox->setEditable(true);
    QFontDatabase db;
    foreach (int size, db.standardSizes()) {
        sizeBox->addItem(QString::number(size));
    }
    connect(sizeBox,SIGNAL(activated(QString)),textEdit,SLOT(setFontSize(QString)));

    //创建列表选项框
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
    connect(listBox,SIGNAL(activated(int)),this,SLOT(setList(int)));
    connect(textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(changeListBox()));
}

void MainWindow::createActions()
{
    //添加文件菜单的动作
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

    closeFileAction=new QAction(QIcon(":/icons/close"),tr("&Close"),this);
    closeFileAction->setShortcut(QKeySequence::Close);
    closeFileAction->setStatusTip(tr("Close the current window."));
    connect(closeFileAction,&QAction::triggered,this,&MainWindow::close);

    //添加编辑菜单的动作
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
    connect(findAction,SIGNAL(triggered(bool)),findDialog,SLOT(show()));

    //添加格式菜单的动作
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

    //用于表示列表选项的动作
    listStyleGroup=new QActionGroup(this);
    standardListAction=new QAction(tr("&Standard"),listStyleGroup);
    standardListAction->setCheckable(true);
    discListAction=new QAction(tr("&Disc"),listStyleGroup);
    discListAction->setCheckable(true);
    circleListAction=new QAction(tr("&Circle"),listStyleGroup);
    circleListAction->setCheckable(true);
    squareListAction=new QAction(tr("&Square"),listStyleGroup);
    squareListAction->setCheckable(true);
    decimalListAction=new QAction(tr("&Demical"),listStyleGroup);
    decimalListAction->setCheckable(true);
    laListAction=new QAction(tr("&Lower Alpha"),listStyleGroup);
    laListAction->setCheckable(true);
    uaListAction=new QAction(tr("&Upper Alpha"),listStyleGroup);
    uaListAction->setCheckable(true);
    lrListAction=new QAction(tr("&Lower Roman"),listStyleGroup);
    laListAction->setCheckable(true);
    urListAction=new QAction(tr("&Upper Roman"),listStyleGroup);
    urListAction->setCheckable(true);
    standardListAction->setChecked(true);
    connect(listStyleGroup,SIGNAL(triggered(QAction*)),this,SLOT(setList(QAction*)));
    connect(textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(changeListAction()));

    //添加图片菜单的动作
    insertImageAction=new QAction(QIcon(":/icons/insertImage"),tr("Insert &Image"),this);
    insertImageAction->setStatusTip(tr("Insert an image from file into the current cursor position"));
    connect(insertImageAction,SIGNAL(triggered(bool)),this,SLOT(insertImage()));

    modifyImageAction=new QAction(tr("Image Size"),this);
    modifyImageAction->setStatusTip("Change the size of the current image");
    connect(modifyImageAction,SIGNAL(triggered(bool)),modifyImageDialog,SLOT(show()));
    modifyImageAction->setEnabled(false);

    //添加表格菜单的动作
    insertTableAction=new QAction(QIcon(":/icons/table"),tr("Insert &Table"),this);
    insertTableAction->setStatusTip(tr("Insert a table"));
    connect(insertTableAction,&QAction::triggered,insertTableDialog,&QDialog::show);

    mergeCellAction=new QAction(QIcon(),tr("&Merge Cells"),this);
    mergeCellAction->setStatusTip(tr("Merge the cells selected by the cursor"));
    connect(mergeCellAction,SIGNAL(triggered(bool)),textEdit,SLOT(mergeCell()));

    tableHAlignGroup=new QActionGroup(this);
    tableLeftAlignAction=new QAction(tr("Left Align"),tableHAlignGroup);
    tableLeftAlignAction->setCheckable(true);
    tableRightAlignAction=new QAction(tr("Right Align"),tableHAlignGroup);
    tableRightAlignAction->setCheckable(true);
    tableHCenterAlignAction=new QAction(tr("Center Align"),tableHAlignGroup);
    tableHCenterAlignAction->setCheckable(true);
    tableJustifyAlignAction=new QAction(tr("Justify Align"),tableHAlignGroup);
    tableJustifyAlignAction->setCheckable(true);
    connect(tableHAlignGroup, SIGNAL(triggered(QAction*)),this,SLOT(setTableHAlign(QAction*)));
    tableLeftAlignAction->setChecked(true);
    tableHAlignGroup->setEnabled(false);
    connect(textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(changeTableAlignAction()));

    changeFontAction(textEdit->currentCharFormat());


}

//完成菜单的创建并在菜单中添加动作
void MainWindow::createMenus()
{
    //文件菜单
    fileMenu=menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(createFileAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(saveAsFileAction);
    fileMenu->addAction(closeFileAction);

    //编辑菜单
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

    //格式菜单
    formatMenu=menuBar()->addMenu("&Format");
    formatMenu->addAction(boldAction);
    formatMenu->addAction(italicAction);
    formatMenu->addAction(underlineAction);
    formatMenu->addAction(fontColorAction);
    formatMenu->addSeparator();
    formatMenu->addActions(alignGroup->actions());
    formatMenu->addSeparator();
    listStyleMenu=formatMenu->addMenu(QIcon(":/icons/list"),tr("&List Style"));
    listStyleMenu->addActions(listStyleGroup->actions());

    //图片菜单
    imageMenu=menuBar()->addMenu("&Image");
    imageMenu->addAction(insertImageAction);
    imageMenu->addSeparator();
    imageMenu->addAction(modifyImageAction);

    //表格菜单
    tableMenu=menuBar()->addMenu("&Table");
    tableMenu->addAction(insertTableAction);
    tableMenu->addSeparator();
    tableMenu->addAction(mergeCellAction);
    tableHAlignMenu=tableMenu->addMenu("&Horizontal Align");
    tableHAlignMenu->addActions(tableHAlignGroup->actions());
}

//创建工具栏并在其上添加元素
void MainWindow::createToolBars()
{
    //文件工具栏
    fileBar=addToolBar("&File");
    fileBar->setObjectName("fileBar");
    fileBar->addAction(createFileAction);
    fileBar->addAction(openFileAction);
    fileBar->addAction(saveFileAction);

    //编辑工具栏
    editBar=addToolBar("&Edit");
    editBar->setObjectName("editBar");
    editBar->addAction(undoAction);
    editBar->addAction(redoAction);
    editBar->addAction(cutAction);
    editBar->addAction(copyAction);
    editBar->addAction(pasteAction);

    //字体工具栏
    fontBar=addToolBar("&Font");
    fontBar->setObjectName("fontBar");
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
    blockBar->setObjectName("blockBar");
    blockBar->addActions(alignGroup->actions());
    blockBar->addWidget(listBox);
}

//新建文件动作的具体实现
void MainWindow::createFile()
{
    MainWindow *newText=new MainWindow(this->parentWidget());
    newText->setAttribute(Qt::WA_DeleteOnClose);
    newText->show();
}

//打开文件动作的具体实现
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

//保存文件动作的具体实现
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

//另存为文件动作的具体实现
void MainWindow::saveAsFile()
{
    QString path=QFileDialog::getSaveFileName(this,tr("Save File"),".",tr("Text File(*.txt);;HTML File(*.html)"));
    if(!path.isEmpty())
        serialFile(path);
    else
        QMessageBox::warning(this,tr("Path"),tr("You did not select any file."));

}

//加载文件的函数
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

//保存（序列化）文件的函数
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

//返回光标所在处的单词
QString MainWindow::textUnderCursor() const
{
    QTextCursor tc = textEdit->textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

//保存当前的窗口布局
void MainWindow::saveSettings()
{
    QSettings settings("./settings.int",QSettings::IniFormat);
    settings.beginGroup("mainWindow");
    settings.setValue("geometry",saveGeometry());
    settings.setValue("state",saveState());
    settings.endGroup();
}

//加载窗口布局
void MainWindow::loadSettings()
{
    QSettings settings("./settings.int",QSettings::IniFormat);
    settings.beginGroup("mainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

//用于在光标所在处的格式发生变化时更改字体工具栏的函数
void MainWindow::changeFontAction(const QTextCharFormat &format)
{
    fontBox->setCurrentFont(format.font());
    sizeBox->setCurrentText(QString::number(format.fontPointSize()));
    boldAction->setChecked(format.font().bold());
    italicAction->setChecked(format.fontItalic());
    underlineAction->setChecked(format.fontUnderline());
    if(format.toImageFormat().isValid())
        modifyImageAction->setEnabled(true);
    else
        modifyImageAction->setEnabled(false);
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

//设置表格对齐格式
void MainWindow::setTableHAlign(QAction *aim)
{
    QTextTable *curTable=textEdit->textCursor().currentTable();
    if(curTable)
    {
        QTextTableFormat curFormat=curTable->format();
        int flag=curFormat.alignment();
        if(aim==tableLeftAlignAction)
            curFormat.setAlignment(QFlags<Qt::AlignmentFlag>((int)Qt::AlignLeft|(0xFFF0&flag)));
        else if(aim==tableRightAlignAction)
            curFormat.setAlignment(QFlags<Qt::AlignmentFlag>((int)Qt::AlignRight|(0xFFF0&flag)));
        else if(aim==tableHCenterAlignAction)
            curFormat.setAlignment(QFlags<Qt::AlignmentFlag>((int)Qt::AlignHCenter|(0xFFF0&flag)));
        else if(aim==tableJustifyAlignAction)
            curFormat.setAlignment(QFlags<Qt::AlignmentFlag>((int)Qt::AlignJustify|(0xFFF0&flag)));
        curTable->setFormat(curFormat);
    }

}

//设置列表项目格式（工具栏版）
void MainWindow::setList(int index)
{
    QTextCursor cursor=textEdit->textCursor();
    if(index>0&&index<9)
    {
        QTextListFormat::Style style=QTextListFormat::Style(-index);
        cursor.beginEditBlock();
        QTextBlockFormat blockFormat=cursor.blockFormat();
        QTextListFormat listFormat;
        if(cursor.currentList())
        {
            listFormat=cursor.currentList()->format();
        }
        else
        {
            listFormat.setIndent(blockFormat.indent()+1);
        }
        listFormat.setStyle(style);
        cursor.createList(listFormat);
        cursor.endEditBlock();
    }
    else
    {
        QTextList *curList=cursor.currentList();
        if(curList&&!curList->isEmpty())
        {
            curList->remove(cursor.block());
        }
    }
    changeListAction();
}

//设置列表项目格式（菜单版）
void MainWindow::setList(QAction *action)
{
    if(action==standardListAction)
        setList(0);
    else if(action==discListAction)
        setList(1);
    else if(action==circleListAction)
        setList(2);
    else if(action==squareListAction)
        setList(3);
    else if(action==decimalListAction)
        setList(4);
    else if(action==laListAction)
        setList(5);
    else if(action==uaListAction)
        setList(6);
    else if(action==lrListAction)
        setList(7);
    else if(action==urListAction)
        setList(8);
    changeListBox();
}

//更新菜单栏上段落对齐选项
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

//更新菜单栏上表格对齐选项
void MainWindow::changeTableAlignAction()
{
    QTextTable *curTable=textEdit->textCursor().currentTable();
    if(curTable)
    {
        tableHAlignGroup->setEnabled(true);
        Qt::Alignment flag=curTable->format().alignment();
        if(flag&Qt::AlignLeft)
            tableLeftAlignAction->setChecked(true);
        else if(flag&Qt::AlignRight)
            tableRightAlignAction->setChecked(true);
        else if(flag&Qt::AlignHCenter)
            tableHCenterAlignAction->setChecked(true);
        else if(flag&Qt::AlignJustify)
            tableJustifyAlignAction->setChecked(true);
    }
    else
    {
        tableHAlignGroup->setEnabled(false);
    }
}



//在鼠标位置变化时更新列表选项框
void MainWindow::changeListBox()
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

//在鼠标位置变化时更新菜单栏中的列表设置
void MainWindow::changeListAction()
{
    QTextList *curList=textEdit->textCursor().currentList();
    if(curList)
    {
        switch(curList->format().style())
        {
        case QTextListFormat::ListDisc:discListAction->setChecked(true);break;
        case QTextListFormat::ListCircle:circleListAction->setChecked(true);break;
        case QTextListFormat::ListSquare:squareListAction->setChecked(true);break;
        case QTextListFormat::ListDecimal:decimalListAction->setChecked(true);break;
        case QTextListFormat::ListLowerAlpha:laListAction->setChecked(true);break;
        case QTextListFormat::ListUpperAlpha:uaListAction->setChecked(true);break;
        case QTextListFormat::ListLowerRoman:lrListAction->setChecked(true);break;
        case QTextListFormat::ListUpperRoman:urListAction->setChecked(true);break;
        }
    }
    else
    {
        standardListAction->setChecked(true);
    }
}

//根据用户输入的文本和选项进行搜索
void MainWindow::takeSearch()
{
    QString aim;
    *findDialog>>aim;
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

//插入图片的具体实现
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

//插入表格的具体实现
void MainWindow::insertTable()
{
    int column=insertTableDialog->verticalNum(),row=insertTableDialog->horizontalNum();
    textEdit->textCursor().insertTable(row,column);
}

//设置图片大小的具体实现
void MainWindow::setImageSize()
{
    QTextImageFormat curImg=textEdit->currentCharFormat().toImageFormat();
    if(curImg.isValid())
    {
        curImg.setHeight(modifyImageDialog->verticalNum());
        curImg.setWidth(modifyImageDialog->horizontalNum());
        textEdit->setCurrentCharFormat(curImg);
    }
    return;
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
                saveSettings();
                event->accept();
                break;
            default:
                event->ignore();

        }
    }
    else
    {
        saveSettings();
        event->accept();
    }

}

