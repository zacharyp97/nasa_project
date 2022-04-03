#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkReply"
#include "QMessageBox"
#include "QString"
#include "QJsonParseError"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"
#include "QJsonDocument"
#include "apifunc.h"
#include "QWidget"
#include "QStyle"
#include "QJsonValue"
#include "string.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //set up tray icon
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->setToolTip("NASA APPLICATION" "\n"
                         "Retrive NASA API");

    QMenu * menu = new QMenu(this);
    QAction * viewWindow = new QAction(tr("Open window"), this);
    QAction * quitAction = new QAction(tr("Quit"), this);


    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    menu->addAction(viewWindow);
    menu->addAction(quitAction);


    trayIcon->setContextMenu(menu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    //perform api calls and retrieve the required data

    //print date
    QDate curDate = QDate::currentDate();
    QString date = "Date: "+curDate.toString() + "\n";


    //display the data in the form
    ui->setupUi(this);


    //set date values for api
    QString apiYear = QString::number(curDate.year());
    QString apiMonth = QString::number(curDate.month());
    //the day must be the day before
    QString apiDay = QString::number(curDate.day());

    if(curDate.month()<10)
    {
        apiMonth.insert(0,"0");
    }

    if(curDate.day()<10)
    {
        apiDay.insert(0,"0");
    }

    //get Asteroid Picture of the Day
    QString strReply = fetchAPI("https://api.nasa.gov/planetary/apod?date="+apiYear+"-"+apiMonth+"-"+apiDay+"&api_key=avATca9BNlLaVcNC09cMNwL4MAwBeXhBcWZh9Ih9");
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    jsonAPOD = jsonResponse.object();

    //set the apod title for the message box
    QString apodTitleMsg = "APOD Title: " + jsonAPOD["title"].toString() + "\n";

    ui->label_apodTitle->setText(apodTitleMsg);

    ui->label_apodDesc->setText("APOD Description : "+jsonAPOD["explanation"].toString());

    ui->label_date->setText(date);

    //this is used to download the APOD image from the url
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &MainWindow::downloadAPODFinished);
    const QUrl url = QUrl(jsonAPOD["url"].toString());
    QNetworkRequest request(url);
    nam->get(request);


    strReply.clear();

    //get Near Earth Object
    QString apiDayNEOW = QString::number(curDate.day());
    strReply = fetchAPI("https://api.nasa.gov/neo/rest/v1/feed?start_date="+apiYear+"-"+apiMonth+"-"+apiDayNEOW+"&end_date="+apiYear+"-"+apiMonth+"-"+apiDay+"&api_key=avATca9BNlLaVcNC09cMNwL4MAwBeXhBcWZh9Ih9");
    jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    jsonNEOW = jsonResponse.object();
    QJsonObject nearEarthObjects = jsonNEOW["near_earth_objects"].toObject();

    QJsonArray neowObjects = nearEarthObjects.value(apiYear+"-"+apiMonth+"-"+apiDay).toArray();
    QJsonObject tempObj;
    QJsonArray closeApproachArr;
    QJsonObject closeApproachObj;
    QJsonObject missDistanceObj;
    QString missDistanceKM;
    QString smallestNEOWName;
    QString smallestNEOWApp;
    double smallestMissDistance = 9999999999999999.999;
    bool isHazardous = false;
    for(unsigned int i=0; i<neowObjects.size(); i++ )
    {

        tempObj = neowObjects.at(i).toObject();
        if(tempObj["is_potentially_hazardous_asteroid"].toBool())
        {
            isHazardous = true;
            closeApproachArr = tempObj["close_approach_data"].toArray();

            for(unsigned int x=0; x<closeApproachArr.size(); x++ )
            {

                closeApproachObj = closeApproachArr.at(x).toObject();
                missDistanceObj = closeApproachObj["miss_distance"].toObject();
                missDistanceKM = missDistanceObj["kilometers"].toString();

                if(smallestMissDistance > missDistanceKM.toDouble())
                {
                    smallestNEOWName = tempObj["name"].toString();
                    smallestNEOWApp = closeApproachObj["close_approach_date_full"].toString();
                    smallestMissDistance = missDistanceKM.toDouble();
                }
            }


        }

    }
    char output[50];
    snprintf(output, 50, "%f", smallestMissDistance);
    QString neowMsg = "NEOW Today: " + QString::number(neowObjects.size()) + "\n";

    //check if there are any hazardous objects before setting the labels
    if(isHazardous)
    {
        qDebug() << "Smallest Miss Distance Name: "<< smallestNEOWName << "\n";
        qDebug() << "Smallest Miss Distance Date & Time: "<< smallestNEOWApp << "\n";
        qDebug() << "Smallest Miss Distance: "<< output << "\n";
        ui->label_neowTitle->setText("NEOW Name: "+smallestNEOWName);
        ui->label_neowDateTime->setText("NEOW Date & Time: "+smallestNEOWApp);
        ui->label_missDistance->setText("NEOW Miss Distance (KM): "+ QString(output));
    }
    else
    {
        qDebug() << "no hazardous NEOW ";
        ui->label_isHazardous->setText("No potentially hazardous NEOW");
    }

    //display the message box
    if(jsonAPOD.contains("url") && jsonNEOW.contains("near_earth_objects"))
    {
        QMessageBox::about(this,"NASA APIs","NASA Data Today: \n" + date + apodTitleMsg + neowMsg);
    }
    else
        return;



}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(this->isVisible()){
        event->ignore();
        this->hide();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);

        trayIcon->showMessage("Tray Program",
                              tr("NASA Application Minimized"),
                              icon,
                              2000);
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:

        if(!this->isVisible()){
            this->show();
        } else {
            this->hide();
        }

        break;
    default:
        break;
    }
}

void MainWindow::downloadAPODFinished(QNetworkReply *reply)
{
    QPixmap pm;
    pm.loadFromData(reply->readAll());
    ui->label_apodImage->setPixmap(pm);
    ui->label_apodImage->setScaledContents( true );
    ui->label_apodImage->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnHDImage_clicked()
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &MainWindow::downloadAPODFinished);
    const QUrl url = QUrl(jsonAPOD["hdurl"].toString());
    QNetworkRequest request(url);
    nam->get(request);
}

