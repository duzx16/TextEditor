#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include "Texteditor.h"
#include <QMainWindow>

class QToolBar;
class QToolButton;
class QAbstractItemModel;
class QLabel;
class QFontComboBox;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    QAbstractItemModel *modelFromFile(const QString& fileName);

    ~MainWindow();
private slots:
protected:
    //文件菜单函数的具体实现
    void openFile();
    void saveFile();
    void createFile();
    void saveAsFile();


    void loadFile(const QString &filename);
    void serialFile(const QString &filename);

    void closeEvent(QCloseEvent *event) override;


private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createButtons();

    QString textUnderCursor() const;


    //菜单
    QMenu *fileMenu;
    QMenu *editMenu;

    //文件菜单的操作
    QAction *createFileAction;
    QAction *openFileAction;
    QAction *saveFileAction;
    QAction *saveAsFileAction;

    //编辑菜单的操作
    QAction *undoAction;
    QAction *redoAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectallAction;

    //工具栏
    QToolBar *fileBar;
    QToolBar *editBar;
    QToolBar *fontBar;

    //工具栏上不是动作的东西
    //字体
    QLabel *fontLabel;
    QFontComboBox *fontBox;
    QLabel *sizeLabel;
    QComboBox *sizeBox;
    QToolButton *boldButton;
    QToolButton *italicButton;
    QToolButton *underlineButton;
    QToolButton *colorButton;
    //保存当前绑定的文件的路径
    QString curFileName;
    //用于实现自动补全的类
    QCompleter *completer;
    //真正实现文本编辑器的类
    TextEditor *textEdit;


};



#endif // TEXT_EDITOR_H
