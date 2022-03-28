#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkReply"
#include "QString"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"
#include "apifunc.h"
#include "subwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDate curDate = QDate::currentDate();
    qDebug() << "Response:" << curDate.toString();
    QString apiYear = QString::number(curDate.year());
    QString apiMonth = QString::number(curDate.month());
    //the day must be the day before
    QString apiDay = QString::number(curDate.day()-1);

    //get Asteroid Picture of the Day
    QString strReply = fetchAPI("https://api.nasa.gov/planetary/apod?date="+apiYear+"-"+apiMonth+"-"+apiDay+"&api_key=avATca9BNlLaVcNC09cMNwL4MAwBeXhBcWZh9Ih9");
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    jsonAPOD = jsonResponse.object();


    strReply.clear();

    //get Near Earth Object
    QString apiDayNEOW = QString::number(curDate.day()-2);
    strReply = fetchAPI("https://api.nasa.gov/neo/rest/v1/feed?start_date="+apiYear+"-"+apiMonth+"-"+apiDayNEOW+"&end_date="+apiYear+"-"+apiMonth+"-"+apiDay+"&api_key=avATca9BNlLaVcNC09cMNwL4MAwBeXhBcWZh9Ih9");
    jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    jsonNEOW = jsonResponse.object();

    //this is used to download the image from the url
//    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
//    connect(nam, &QNetworkAccessManager::finished, this, &MainWindow::downloadFinished);
//    const QUrl url = QUrl(jsonAPOD["url"].toString());
//    QNetworkRequest request(url);
//    nam->get(request);

}

void MainWindow::downloadFinished(QNetworkReply *reply)
{
    QPixmap pm;
    pm.loadFromData(reply->readAll());
    ui->label_pic->setPixmap(pm);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnHDImage_clicked()
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &MainWindow::downloadFinished);
    const QUrl url = QUrl(jsonAPOD["hdurl"].toString());
    QNetworkRequest request(url);
    nam->get(request);
}

