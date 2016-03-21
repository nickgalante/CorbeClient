#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QStringListModel>


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

     void on_pushButton_clicked();

     void on_getFileButton_clicked();

     void doDownload();

     void updateProgress(qint64, qint64);

     void on_fileList_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
};

#endif // MAINWINDOW_H
