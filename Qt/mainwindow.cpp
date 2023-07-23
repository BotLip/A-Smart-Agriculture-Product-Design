<<<<<<< HEAD
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    naManager = new QNetworkAccessManager(this);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    Q_ASSERT(connRet);

    getATocken();

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData()));
    m_timer->start(1000);
}

MainWindow::~MainWindow()
{
    m_timer->stop();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{


}

QString UtilFromJsonArrayToString(const QJsonArray &data) {
    QJsonDocument doc;
    doc.setArray(data);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact).constData());
}

QString UtilFromJsonObjectToString(const QJsonObject &data) {
    QString strRet;
    QJsonDocument doc(data);
    strRet = QString(doc.toJson(QJsonDocument::Indented));
    return strRet;
}

void MainWindow::requestFinished(QNetworkReply* reply) {
    // 获取http状态码
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    }
    else {
        // 获取返回内容
        if(ATocken.isEmpty()){
            ATocken = reply->rawHeader("X-Subject-Token");
            qDebug()<<ATocken;
        }
        else
        {

            QByteArray response = reply->readAll();
            //qDebug()<<response;
            QJsonDocument jsondoc = QJsonDocument::fromJson(response);

            QJsonObject jobj = jsondoc.object();
            QJsonArray jobjshadow = jobj["shadow"].toArray();
            //qDebug()<<UtilFromJsonArrayToString(jobjshadow);
            for (int i = 0; i < jobjshadow.size(); i++) {
               QJsonObject jsonObject = jobjshadow.at(i).toObject();
               QJsonObject jsonObjectreported=jsonObject["reported"].toObject();
               QJsonObject jsonObjectproperties=jsonObjectreported["properties"].toObject();
               //qDebug()<<UtilFromJsonObjectToString(jsonObjectproperties);
               //int Humidity=jsonObjectproperties["humi"].toInt();
               QString Humidity =jsonObjectproperties["humi"].toString();
               QString Temperature =jsonObjectproperties["temp"].toString();
               //int Temperature=jsonObjectproperties["temp"].toInt();
               int luminance=jsonObjectproperties["lumin"].toInt();
               int distance=jsonObjectproperties["dist"].toInt();
               int LampState=jsonObjectproperties["LampSt"].toInt();
               int MotorState=jsonObjectproperties["devSt"].toInt();
               qDebug()<<Humidity<<" "<<Temperature<<" "<<luminance;

               ui->humi->setText(Humidity);
               ui->temp->setText(Temperature);
               ui->lumi->setText(QString::number(luminance));
               ui->dist->setText(QString::number(distance));
               ui->LampSt->setText(QString::number(LampState));
               ui->devSt->setText(QString::number(MotorState));

            }
        }
    }
}

void MainWindow::updateData()
{
     qDebug()<<"updateData";
     getCloundData();
}

void MainWindow::getATocken()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://iam.cn-north-4.myhuaweicloud.com/v3/auth/tokens";
    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    const char *data = "{ \r\n    \"auth\": "
                       "{ \r\n        \"identity\": "
                       "{ \r\n            \"methods\": "
                       "[ \r\n                \"password\" \r\n            ], \r\n            \"password\":"
                       " { \r\n                \"user\":"
                       " { \r\n                    \"name\": \"panda\", \r\n                    "
                       "\"password\": \"112233lzr\", \r\n                    "
                       "\"Domain\": { \r\n                        "
                       "\"name\": \"hw064869346\" \r\n                    } \r\n                } \r\n            } \r\n        }, "
                       "\r\n        \"scope\":"
                       " { \r\n            \"project\": "
                       "{ \r\n                \"name\": \"cn-north-4\" \r\n            } \r\n        } \r\n    } \r\n}";
    naManager->post(request, data);
}

void MainWindow::getCloundData()
{
    if(ATocken.isEmpty()){
        return;
    }

    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/shadow";
    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    naManager->get(request);
}

void MainWindow::on_remoteOpenLight_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Light\",\n  \"paras\" : {\n    \"light\" : \"ON\"\n  }}";
    naManager->post(request, data);
}


void MainWindow::on_remoteCloseLight_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";
    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    //const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Light\",\n  \"paras\" : {\n    \"light\" : \"OFF\"\n  },\n  \"expire_time\" : 0,\n  \"send_strategy\" : \"immediately\"\n}";
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Light\",\n  \"paras\" : {\n    \"light\" : \"OFF\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteOpenMotor_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Motor\",\n  \"paras\" : {\n    \"Motor\" : \"ON\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteCloseMotor_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Motor\",\n  \"paras\" : {\n    \"Motor\" : \"OFF\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteOpenAutoMode_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"AutoMode\",\n  \"paras\" : {\n    \"AutoMode\" : \"ON\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteCloseAutoMode_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"AutoMode\",\n  \"paras\" : {\n    \"AutoMode\" : \"OFF\"\n  }}";
    naManager->post(request, data);
}
=======
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    naManager = new QNetworkAccessManager(this);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    Q_ASSERT(connRet);

    getATocken();

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData()));
    m_timer->start(1000);
}

MainWindow::~MainWindow()
{
    m_timer->stop();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{


}

QString UtilFromJsonArrayToString(const QJsonArray &data) {
    QJsonDocument doc;
    doc.setArray(data);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact).constData());
}

QString UtilFromJsonObjectToString(const QJsonObject &data) {
    QString strRet;
    QJsonDocument doc(data);
    strRet = QString(doc.toJson(QJsonDocument::Indented));
    return strRet;
}

void MainWindow::requestFinished(QNetworkReply* reply) {
    // 获取http状态码
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    }
    else {
        // 获取返回内容
        if(ATocken.isEmpty()){
            ATocken = reply->rawHeader("X-Subject-Token");
            qDebug()<<ATocken;
        }
        else
        {

            QByteArray response = reply->readAll();
            //qDebug()<<response;
            QJsonDocument jsondoc = QJsonDocument::fromJson(response);

            QJsonObject jobj = jsondoc.object();
            QJsonArray jobjshadow = jobj["shadow"].toArray();
            //qDebug()<<UtilFromJsonArrayToString(jobjshadow);
            for (int i = 0; i < jobjshadow.size(); i++) {
               QJsonObject jsonObject = jobjshadow.at(i).toObject();
               QJsonObject jsonObjectreported=jsonObject["reported"].toObject();
               QJsonObject jsonObjectproperties=jsonObjectreported["properties"].toObject();
               //qDebug()<<UtilFromJsonObjectToString(jsonObjectproperties);
               //int Humidity=jsonObjectproperties["humi"].toInt();
               QString Humidity =jsonObjectproperties["humi"].toString();
               QString Temperature =jsonObjectproperties["temp"].toString();
               //int Temperature=jsonObjectproperties["temp"].toInt();
               int luminance=jsonObjectproperties["lumin"].toInt();
               int distance=jsonObjectproperties["dist"].toInt();
               int LampState=jsonObjectproperties["LampSt"].toInt();
               int MotorState=jsonObjectproperties["devSt"].toInt();
               qDebug()<<Humidity<<" "<<Temperature<<" "<<luminance;

               ui->humi->setText(Humidity);
               ui->temp->setText(Temperature);
               ui->lumi->setText(QString::number(luminance));
               ui->dist->setText(QString::number(distance));
               ui->LampSt->setText(QString::number(LampState));
               ui->devSt->setText(QString::number(MotorState));

            }
        }
    }
}

void MainWindow::updateData()
{
     qDebug()<<"updateData";
     getCloundData();
}

void MainWindow::getATocken()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://iam.cn-north-4.myhuaweicloud.com/v3/auth/tokens";
    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    const char *data = "{ \r\n    \"auth\": "
                       "{ \r\n        \"identity\": "
                       "{ \r\n            \"methods\": "
                       "[ \r\n                \"password\" \r\n            ], \r\n            \"password\":"
                       " { \r\n                \"user\":"
                       " { \r\n                    \"name\": \"panda\", \r\n                    "
                       "\"password\": \"112233lzr\", \r\n                    "
                       "\"Domain\": { \r\n                        "
                       "\"name\": \"hw064869346\" \r\n                    } \r\n                } \r\n            } \r\n        }, "
                       "\r\n        \"scope\":"
                       " { \r\n            \"project\": "
                       "{ \r\n                \"name\": \"cn-north-4\" \r\n            } \r\n        } \r\n    } \r\n}";
    naManager->post(request, data);
}

void MainWindow::getCloundData()
{
    if(ATocken.isEmpty()){
        return;
    }

    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/shadow";
    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    naManager->get(request);
}

void MainWindow::on_remoteOpenLight_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Light\",\n  \"paras\" : {\n    \"light\" : \"ON\"\n  }}";
    naManager->post(request, data);
}


void MainWindow::on_remoteCloseLight_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";
    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    //const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Light\",\n  \"paras\" : {\n    \"light\" : \"OFF\"\n  },\n  \"expire_time\" : 0,\n  \"send_strategy\" : \"immediately\"\n}";
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Light\",\n  \"paras\" : {\n    \"light\" : \"OFF\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteOpenMotor_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Motor\",\n  \"paras\" : {\n    \"Motor\" : \"ON\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteCloseMotor_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"Motor\",\n  \"paras\" : {\n    \"Motor\" : \"OFF\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteOpenAutoMode_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"AutoMode\",\n  \"paras\" : {\n    \"AutoMode\" : \"ON\"\n  }}";
    naManager->post(request, data);
}

void MainWindow::on_remoteCloseAutoMode_clicked()
{
    QNetworkRequest request;
    QString requestUrl;
    //请求地址
    requestUrl="https://ee55e46a8d.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/01e240a4c32d42c781e2c5cecd2fdbc5/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/commands";

    //设置请求地址
    QUrl url;
    url.setUrl(requestUrl);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    //qDebug()<<ATocken;
    request.setRawHeader("User-Agent", "API Explorer");
    request.setRawHeader("X-Auth-Token", ATocken.toUtf8());
    const char *data = "{\n  \"service_id\" : \"64a389ee9415fc7a573b7ef3\",\n  \"command_name\" :\"AutoMode\",\n  \"paras\" : {\n    \"AutoMode\" : \"OFF\"\n  }}";
    naManager->post(request, data);
}
>>>>>>> a25bce24006dac63859a9ad3fff9578815bd781f
