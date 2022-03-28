#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QJsonObject"
#include "QtNetwork/QNetworkAccessManager"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QJsonObject jsonAPOD;
    QJsonObject jsonNEOW;

    MainWindow(QWidget *parent = nullptr);
    void downloadFinished(QNetworkReply *reply);
    ~MainWindow();

private slots:
    void on_btnHDImage_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
