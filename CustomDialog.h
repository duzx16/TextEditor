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
    QPushButton *findAction(){return findButton;}
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

class InsertTableDialog:public QDialog
{
    Q_OBJECT
public:
    InsertTableDialog(QWidget *parent=0);
    int colunm();
    int row();
private:
    QLabel *sizeLabel;
    QLabel *columnLabel;
    QSpinBox *columnBox;
    QLabel *rowLabel;
    QSpinBox *rowBox;
    QDialogButtonBox *buttonBox;

};

#endif // FINDDIALOG_H
