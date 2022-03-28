#include "./ui_mainwindow.h"
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkReply"
#include "QString"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"

QString fetchAPI(QString reqURL)
{
        //this is used to parse the json
        QEventLoop eventLoop;
        QNetworkAccessManager *mgr = new QNetworkAccessManager;
        QObject::connect(mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        QNetworkRequest req( QUrl( reqURL.toUtf8()) );
    //    QNetworkRequest req( QUrl( QString("https://api.nasa.gov/neo/rest/v1/feed?start_date=2022-03-27&end_date=2022-03-27&api_key=avATca9BNlLaVcNC09cMNwL4MAwBeXhBcWZh9Ih9") ) );

        QNetworkReply *reply = mgr->get(req);
        eventLoop.exec();

        QString strReply = (QString)reply->readAll();
        qDebug() << "Response:" << strReply;

        return strReply;
}
