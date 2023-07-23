<<<<<<< HEAD
/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *remoteOpenLight;
    QPushButton *remoteCloseLight;
    QLabel *temp;
    QLabel *humi;
    QLabel *lumi;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 800);
        MainWindow->setMinimumSize(QSize(800, 0));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(90, 30, 171, 41));
        label->setStyleSheet(QString::fromUtf8("font: 20pt \"Arial\";"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(450, 30, 171, 41));
        label_2->setStyleSheet(QString::fromUtf8("font: 20pt \"Arial\";"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(770, 30, 131, 41));
        label_3->setStyleSheet(QString::fromUtf8("font: 20pt \"Arial\";"));
        remoteOpenLight = new QPushButton(centralwidget);
        remoteOpenLight->setObjectName(QString::fromUtf8("remoteOpenLight"));
        remoteOpenLight->setGeometry(QRect(200, 210, 231, 81));
        remoteOpenLight->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";\n"
""));
        remoteCloseLight = new QPushButton(centralwidget);
        remoteCloseLight->setObjectName(QString::fromUtf8("remoteCloseLight"));
        remoteCloseLight->setGeometry(QRect(610, 210, 201, 81));
        remoteCloseLight->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        temp = new QLabel(centralwidget);
        temp->setObjectName(QString::fromUtf8("temp"));
        temp->setGeometry(QRect(90, 100, 231, 41));
        temp->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        humi = new QLabel(centralwidget);
        humi->setObjectName(QString::fromUtf8("humi"));
        humi->setGeometry(QRect(450, 100, 161, 41));
        humi->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        lumi = new QLabel(centralwidget);
        lumi->setObjectName(QString::fromUtf8("lumi"));
        lumi->setGeometry(QRect(770, 100, 181, 41));
        lumi->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1024, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "\346\270\251\345\272\246", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\346\271\277\345\272\246", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\344\272\256\345\272\246", nullptr));
        remoteOpenLight->setText(QApplication::translate("MainWindow", "\350\277\234\347\250\213\345\274\200\347\201\257", nullptr));
        remoteCloseLight->setText(QApplication::translate("MainWindow", "\350\277\234\347\250\213\345\205\263\347\201\257", nullptr));
        temp->setText(QApplication::translate("MainWindow", "0", nullptr));
        humi->setText(QApplication::translate("MainWindow", "0", nullptr));
        lumi->setText(QApplication::translate("MainWindow", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
=======
/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *remoteOpenLight;
    QPushButton *remoteCloseLight;
    QLabel *temp;
    QLabel *humi;
    QLabel *lumi;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 800);
        MainWindow->setMinimumSize(QSize(800, 0));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(90, 30, 171, 41));
        label->setStyleSheet(QString::fromUtf8("font: 20pt \"Arial\";"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(450, 30, 171, 41));
        label_2->setStyleSheet(QString::fromUtf8("font: 20pt \"Arial\";"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(770, 30, 131, 41));
        label_3->setStyleSheet(QString::fromUtf8("font: 20pt \"Arial\";"));
        remoteOpenLight = new QPushButton(centralwidget);
        remoteOpenLight->setObjectName(QString::fromUtf8("remoteOpenLight"));
        remoteOpenLight->setGeometry(QRect(200, 210, 231, 81));
        remoteOpenLight->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";\n"
""));
        remoteCloseLight = new QPushButton(centralwidget);
        remoteCloseLight->setObjectName(QString::fromUtf8("remoteCloseLight"));
        remoteCloseLight->setGeometry(QRect(610, 210, 201, 81));
        remoteCloseLight->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        temp = new QLabel(centralwidget);
        temp->setObjectName(QString::fromUtf8("temp"));
        temp->setGeometry(QRect(90, 100, 231, 41));
        temp->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        humi = new QLabel(centralwidget);
        humi->setObjectName(QString::fromUtf8("humi"));
        humi->setGeometry(QRect(450, 100, 161, 41));
        humi->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        lumi = new QLabel(centralwidget);
        lumi->setObjectName(QString::fromUtf8("lumi"));
        lumi->setGeometry(QRect(770, 100, 181, 41));
        lumi->setStyleSheet(QString::fromUtf8("font: 16pt \"Arial\";"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1024, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "\346\270\251\345\272\246", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\346\271\277\345\272\246", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\344\272\256\345\272\246", nullptr));
        remoteOpenLight->setText(QApplication::translate("MainWindow", "\350\277\234\347\250\213\345\274\200\347\201\257", nullptr));
        remoteCloseLight->setText(QApplication::translate("MainWindow", "\350\277\234\347\250\213\345\205\263\347\201\257", nullptr));
        temp->setText(QApplication::translate("MainWindow", "0", nullptr));
        humi->setText(QApplication::translate("MainWindow", "0", nullptr));
        lumi->setText(QApplication::translate("MainWindow", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
>>>>>>> a25bce24006dac63859a9ad3fff9578815bd781f
