#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include "Texteditor.h"
#include "CustomDialog.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QToolBar;
class QToolButton;
class QAbstractItemModel;
class QLabel;
class QFontComboBox;
class QComboBox;
class QTextCharFormat;
class QActionGroup;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = 0);
    QAbstractItemModel *modelFromFile(const QString& fileName);
    ~MainWindow();
public slots:
    //文件菜单函数的具体实现
    void openFile();
    void saveFile();
    void createFile();
    void saveAsFile();

    //编辑菜单查找功能的具体实现
    void takeSearch();

    //格式菜单功能的具体实现
    void setBlockAlign(QAction *aim);


    //与列表有关的具体实现函数
    void setList(int index);
    void setList(QAction *action);

    //图片菜单函数的具体实现
    void insertImage();
    void setImageSize();

    //表格菜单函数的具体实现
    void insertTable();
    void setTableHAlign(QAction *aim);

    //用于自动更新界面上的某些选项
    void changeFontAction(const QTextCharFormat &format);
    void changeAlignAction();
    void changeTableAlignAction();
    void changeListBox();
    void changeListAction();



protected:
    void closeEvent(QCloseEvent *event) override;
private:
    //创建GUI中的具体元素
    void createActions();
    void createMenus();
    void createToolBars();
    void createButtons();

    //两个负责文件读写具体实现的底层函数
    void loadFile(const QString &filename);
    void serialFile(const QString &filename);

    QString textUnderCursor() const;

    //用于保存和载入窗口的布局
    void saveSettings();
    void loadSettings();


    //菜单
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *formatMenu;
    QMenu *listStyleMenu;
    QMenu *imageMenu;
    QMenu *tableMenu;
    QMenu *tableHAlignMenu;
    QMenu *tableVAlignMenu;

    //文件菜单的操作
    QAction *createFileAction;
    QAction *openFileAction;
    QAction *saveFileAction;
    QAction *saveAsFileAction;
    QAction *closeFileAction;

    //编辑菜单的操作
    QAction *undoAction;
    QAction *redoAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectallAction;

    QAction *findAction;

    //格式菜单的操作
    //字体
    QAction *boldAction;
    QAction *italicAction;
    QAction *underlineAction;
    QAction *fontColorAction;

    //对齐
    QActionGroup *alignGroup;
    QAction *leftAlignAction;
    QAction *rightAlignAction;
    QAction *centerAlignAction;
    QAction *justifyAlignAction;

    //列表
    QActionGroup *listStyleGroup;
    QAction *standardListAction;
    QAction *discListAction;
    QAction *circleListAction;
    QAction *squareListAction;
    QAction *decimalListAction;
    QAction *laListAction;
    QAction *uaListAction;
    QAction *lrListAction;
    QAction *urListAction;

    //图片菜单的操作
    QAction *insertImageAction;
    QAction *modifyImageAction;

    //表格菜单的操作
    QAction *insertTableAction;
    QAction *mergeCellAction;
    //表格的对齐
    QActionGroup *tableHAlignGroup;
    QAction *tableLeftAlignAction;
    QAction *tableRightAlignAction;
    QAction *tableHCenterAlignAction;
    QAction *tableJustifyAlignAction;

    //工具栏
    QToolBar *fileBar;
    QToolBar *editBar;
    QToolBar *fontBar;
    QToolBar *blockBar;

    //工具栏上不是动作的东西
    //字体
    QLabel *fontLabel;
    QFontComboBox *fontBox;
    QLabel *sizeLabel;
    QComboBox *sizeBox;
    //段落
    QLabel *listLabel;
    QComboBox *listBox;


    //保存当前绑定的文件的路径
    QString curFileName;
    //用于实现自动补全的类
    QCompleter *completer;
    //查找对话框
    FindDialog *findDialog;
    //插入表格对话框
    BiNumberDialog *insertTableDialog;
    //修改图片大小对话框
    BiNumberDialog *modifyImageDialog;
    //真正实现文本编辑器的类
    TextEditor *textEdit;


};



#endif // TEXT_EDITOR_H
