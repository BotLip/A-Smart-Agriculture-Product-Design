<<<<<<< HEAD
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QTimer>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void getCloundData();

private slots:
    void on_pushButton_clicked();
    void requestFinished(QNetworkReply* reply);
    void updateData();
    void getATocken();
    void on_remoteOpenLight_clicked();
    void on_remoteCloseLight_clicked();
    void on_remoteOpenMotor_clicked();
    void on_remoteCloseMotor_clicked();
    void on_remoteOpenAutoMode_clicked();
    void on_remoteCloseAutoMode_clicked();
private:
    Ui::MainWindow *ui;
    QString ATocken;
    QTimer* m_timer;
    QNetworkAccessManager* naManager;
};
#endif // MAINWINDOW_H
=======
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QTimer>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void getCloundData();

private slots:
    void on_pushButton_clicked();
    void requestFinished(QNetworkReply* reply);
    void updateData();
    void getATocken();
    void on_remoteOpenLight_clicked();
    void on_remoteCloseLight_clicked();
    void on_remoteOpenMotor_clicked();
    void on_remoteCloseMotor_clicked();
    void on_remoteOpenAutoMode_clicked();
    void on_remoteCloseAutoMode_clicked();
private:
    Ui::MainWindow *ui;
    QString ATocken;
    QTimer* m_timer;
    QNetworkAccessManager* naManager;
};
#endif // MAINWINDOW_H
>>>>>>> a25bce24006dac63859a9ad3fff9578815bd781f
