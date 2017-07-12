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

#endif // FINDDIALOG_H
