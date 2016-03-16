#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);



    ~MainWindow();

private slots:
     void handleLogin();
     void replyFinished(QNetworkReply*);

     void on_backToLogin_clicked();

     void on_loginButtonBox_accepted();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
