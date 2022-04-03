#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QJsonObject"
#include "QtNetwork/QNetworkAccessManager"
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>

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
    void downloadAPODFinished(QNetworkReply *reply);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent * event);

private slots:
    void on_btnHDImage_clicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon * trayIcon;
};

#endif // MAINWINDOW_H
