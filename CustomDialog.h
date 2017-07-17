#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QString;
class QSpinBox;

QT_END_NAMESPACE

//查找对话框
class FindDialog : public QDialog
{
    Q_OBJECT
public:
    FindDialog(QWidget *parent=0);
    void setLastSearch();
    void clearLastSearch();
    bool isNewSearch();
    QString searchContent();
    bool fromStart();
    bool caseCheck();
    bool wholeWords();
    bool useReg();
    bool backward();
    FindDialog &operator>>(QString &aim);
signals:
    void find();
private:
    QLabel *label;
    QLineEdit *lineEdit;
    QCheckBox *caseCheckBox;
    QCheckBox *fromStartCheckBox;
    QCheckBox *wholeWordsCheckBox;
    QCheckBox *useRegCheckBox;
    QCheckBox *backwardCheckBox;
    QDialogButtonBox *buttonBox;
    QPushButton *findButton;
    QPushButton *moreButton;
    QWidget *extension;
    QString lastSearch;
};

//虚基类，提供了一个有两个整数输入框的对话框接口
class BiNumberDialog:public QDialog
{
    Q_OBJECT
public:
    BiNumberDialog(QWidget *parent=0);
    virtual int verticalNum()=0;
    virtual int horizontalNum()=0;
    virtual ~BiNumberDialog();
};

//具体实现：设置表格行数列数对话框
class InsertTableDialog:public BiNumberDialog
{
    Q_OBJECT
public:
    InsertTableDialog(QWidget *parent=0);
    int verticalNum();
    int horizontalNum();
private:
    QLabel *sizeLabel;
    QLabel *columnLabel;
    QSpinBox *columnBox;
    QLabel *rowLabel;
    QSpinBox *rowBox;
    QDialogButtonBox *buttonBox;

};

//具体实现：设置图片长宽的对话框
class ModifyImageDialog:public BiNumberDialog
{
    Q_OBJECT
public:
    ModifyImageDialog(QWidget *parent=0);
    int verticalNum();
    int horizontalNum();
private:
    QLabel *sizeLabel;
    QLabel *heightLabel;
    QSpinBox *heightBox;
    QLabel *widthLabel;
    QSpinBox *widthBox;
    QDialogButtonBox *buttonBox;
};

#endif // FINDDIALOG_H
