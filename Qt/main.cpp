<<<<<<< HEAD
#include "mainwindow.h"
#include <windows.h>
#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMetaObject>
#include <QEventLoop>
#include <QJsonDocument>            //以下是json数据传送所需头文件
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
      QPixmap pixmap(":/jiazai.jpg");
      QSplashScreen splash(pixmap);
      splash.show();
      splash.showMessage("程序正在加载......", Qt::AlignTop, Qt::red); //显示文字
      Sleep(1000);


      MainWindow w;
      w.show();
      splash.finish(&w);
      return a.exec();
}
=======
#include "mainwindow.h"
#include <windows.h>
#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMetaObject>
#include <QEventLoop>
#include <QJsonDocument>            //以下是json数据传送所需头文件
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
      QPixmap pixmap(":/jiazai.jpg");
      QSplashScreen splash(pixmap);
      splash.show();
      splash.showMessage("程序正在加载......", Qt::AlignTop, Qt::red); //显示文字
      Sleep(1000);


      MainWindow w;
      w.show();
      splash.finish(&w);
      return a.exec();
}
>>>>>>> a25bce24006dac63859a9ad3fff9578815bd781f
