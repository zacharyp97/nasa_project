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

        QNetworkReply *reply = mgr->get(req);
        eventLoop.exec();

        QString strReply = (QString)reply->readAll();
//        qDebug() << "Response:" << strReply;

        return strReply;
}

