#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QMainWindow>

namespace Ui {
class SubWindow;
}

class SubWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SubWindow(QWidget *parent = nullptr);
    ~SubWindow();

private:
    Ui::SubWindow *ui;
};

#endif // SUBWINDOW_H
