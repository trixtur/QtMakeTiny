/********************************************************************************
** Form generated from reading UI file 'maketiny.ui'
**
** Created: Tue Dec 21 16:09:04 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAKETINY_H
#define UI_MAKETINY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MakeTiny
{
public:
    QAction *actionE_xit;
    QAction *action_About;
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QLineEdit *tinyUrl_output;
    QLineEdit *inputUrl;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QComboBox *serviceCombo;
    QPushButton *mktny_button;
    QLabel *label;
    QWidget *tab_2;
    QLineEdit *tinyURL_Input;
    QPushButton *rev_button;
    QTextEdit *LongURL_Output;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Help;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MakeTiny)
    {
        if (MakeTiny->objectName().isEmpty())
            MakeTiny->setObjectName(QString::fromUtf8("MakeTiny"));
        MakeTiny->resize(302, 277);
        actionE_xit = new QAction(MakeTiny);
        actionE_xit->setObjectName(QString::fromUtf8("actionE_xit"));
        action_About = new QAction(MakeTiny);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        centralWidget = new QWidget(MakeTiny);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 10, 301, 221));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        tinyUrl_output = new QLineEdit(tab);
        tinyUrl_output->setObjectName(QString::fromUtf8("tinyUrl_output"));
        tinyUrl_output->setEnabled(true);
        tinyUrl_output->setGeometry(QRect(12, 149, 151, 20));
        tinyUrl_output->setAutoFillBackground(true);
        tinyUrl_output->setReadOnly(true);
        inputUrl = new QLineEdit(tab);
        inputUrl->setObjectName(QString::fromUtf8("inputUrl"));
        inputUrl->setGeometry(QRect(10, 20, 251, 20));
        inputUrl->setAcceptDrops(true);
        layoutWidget = new QWidget(tab);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 50, 183, 25));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        serviceCombo = new QComboBox(layoutWidget);
        serviceCombo->setObjectName(QString::fromUtf8("serviceCombo"));
        serviceCombo->setMinimumSize(QSize(100, 0));
        serviceCombo->setMaximumSize(QSize(69, 16777215));
        serviceCombo->setAcceptDrops(false);

        horizontalLayout->addWidget(serviceCombo);

        mktny_button = new QPushButton(layoutWidget);
        mktny_button->setObjectName(QString::fromUtf8("mktny_button"));

        horizontalLayout->addWidget(mktny_button);

        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 170, 131, 16));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tinyURL_Input = new QLineEdit(tab_2);
        tinyURL_Input->setObjectName(QString::fromUtf8("tinyURL_Input"));
        tinyURL_Input->setGeometry(QRect(10, 20, 171, 20));
        rev_button = new QPushButton(tab_2);
        rev_button->setObjectName(QString::fromUtf8("rev_button"));
        rev_button->setGeometry(QRect(190, 20, 101, 23));
        LongURL_Output = new QTextEdit(tab_2);
        LongURL_Output->setObjectName(QString::fromUtf8("LongURL_Output"));
        LongURL_Output->setEnabled(false);
        LongURL_Output->setGeometry(QRect(10, 50, 281, 141));
        LongURL_Output->setReadOnly(true);
        tabWidget->addTab(tab_2, QString());
        MakeTiny->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MakeTiny);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 302, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        MakeTiny->setMenuBar(menuBar);
        statusBar = new QStatusBar(MakeTiny);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MakeTiny->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addSeparator();
        menu_File->addAction(actionE_xit);
        menu_Help->addAction(action_About);

        retranslateUi(MakeTiny);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MakeTiny);
    } // setupUi

    void retranslateUi(QMainWindow *MakeTiny)
    {
        MakeTiny->setWindowTitle(QApplication::translate("MakeTiny", "MakeTiny", 0, QApplication::UnicodeUTF8));
        actionE_xit->setText(QApplication::translate("MakeTiny", "E&xit", 0, QApplication::UnicodeUTF8));
        actionE_xit->setShortcut(QApplication::translate("MakeTiny", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        action_About->setText(QApplication::translate("MakeTiny", "&About", 0, QApplication::UnicodeUTF8));
        mktny_button->setText(QApplication::translate("MakeTiny", "Make Tiny", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MakeTiny", "Double-Click URL to copy", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MakeTiny", "Make Tiny", 0, QApplication::UnicodeUTF8));
        rev_button->setText(QApplication::translate("MakeTiny", "Reverse Lookup", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MakeTiny", "Reverse", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MakeTiny", "&File", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MakeTiny", "&Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MakeTiny: public Ui_MakeTiny {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAKETINY_H
