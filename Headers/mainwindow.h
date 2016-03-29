#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serverinterface.h"
#include <QMainWindow>
#include <QNetworkReply>
#include <QStringListModel>
#include <QStandardItemModel>


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
     //void replyFinished(QNetworkReply*);
     void requestReceived(QNetworkReply*);
     void displayMessage(QString msg);

     void on_backToLogin_clicked();

     void on_getFileButton_clicked();

     void doDownload();

     void updateProgress(qint64, qint64);

     void on_fileChooseButton_clicked();

     void on_fileList_clicked(const QModelIndex &index);

//     void on_pushButton_clicked();

     void on_downloadFileButton_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    //ServerInterface* si;
};

#endif // MAINWINDOW_H
