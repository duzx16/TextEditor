#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include "texteditor.h"
#include <QMainWindow>



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    QAbstractItemModel *modelFromFile(const QString& fileName);

    ~MainWindow();
private slots:
protected:
    void openFile();
    void saveFile();
    void createFile();
    void saveAsFile();


    void loadFile(const QString &filename);
    void serialFile(const QString &filename);

    void closeEvent(QCloseEvent *event) override;

    //覆盖这两个事件以便于自动补全的操作

private:
    void createActions();
    void createMenus();
    void createToolBars();

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

    //保存当前绑定的文件的路径
    QString curFileName;
    //用于实现自动补全的类
    QCompleter *completer;
    //真正实现文本编辑器的类
    TextEditor *textEdit;


};



#endif // TEXT_EDITOR_H
