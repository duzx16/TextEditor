#include "FindDialog.h"
#include <QtWidgets>

FindDialog::FindDialog(QWidget *parent):QDialog(parent)
{
    label=new QLabel(tr("Find &what"),this);
    lineEdit=new QLineEdit(this);
    label->setBuddy(lineEdit);

    caseCheckBox=new QCheckBox(tr("Match &case"),this);
    fromStartCheckBox=new QCheckBox(tr("Search from &start"),this);
    fromStartCheckBox->setChecked(true);

    findButton=new QPushButton(tr("&Find"),this);
    findButton->setDefault(true);

    moreButton=new QPushButton(tr("&More"),this);
    moreButton->setCheckable(true);
    moreButton->setAutoDefault(false);

    extension=new QWidget(this);

    wholeWordsCheckBox = new QCheckBox(tr("&Whole words"));
    backwardCheckBox = new QCheckBox(tr("Search &backward"));
    useRegCheckBox = new QCheckBox(tr("&Use regular expression"));

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(moreButton, QDialogButtonBox::ActionRole);

    connect(moreButton, &QAbstractButton::toggled, extension, &QWidget::setVisible);

    QVBoxLayout *extensionLayout = new QVBoxLayout;
    extensionLayout->setMargin(0);
    extensionLayout->addWidget(wholeWordsCheckBox);
    extensionLayout->addWidget(backwardCheckBox);
    extensionLayout->addWidget(useRegCheckBox);
    extension->setLayout(extensionLayout);

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(lineEdit);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addWidget(fromStartCheckBox);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(leftLayout, 0, 0);
    mainLayout->addWidget(buttonBox, 0, 1);
    mainLayout->addWidget(extension, 1, 0, 1, 2);
    mainLayout->setRowStretch(2, 1);

    setLayout(mainLayout);

    setWindowTitle(tr("Find"));

    extension->hide();

}

void FindDialog::setLastSearch()
{
    lastSearch=lineEdit->text();
}

bool FindDialog::isNewSearch()
{
    return lineEdit->text()==lastSearch;
}

QString FindDialog::searchContent()
{
    return lineEdit->text();
}

bool FindDialog::fromStart()
{
    return fromStartCheckBox->isChecked();
}

bool FindDialog::caseCheck()
{
    return caseCheckBox->isChecked();
}

bool FindDialog::wholeWords()
{
    return wholeWordsCheckBox->isChecked();
}

bool FindDialog::useReg()
{
    return useRegCheckBox->isChecked();
}

bool FindDialog::backward()
{
    return backwardCheckBox->isChecked();
}
