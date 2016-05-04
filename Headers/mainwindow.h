#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serverinterface.h"
#include <QMainWindow>
#include <QNetworkReply>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>


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
     void handleSignout(QString msg);
     //void replyFinished(QNetworkReply*);
     //void requestReceived(QNetworkReply*);
     void displayMessage(QString msg);
     void displayServerIsNotContactable();

     void on_backToLogin_clicked();

     void on_getFileListButton_clicked();

     void doDownload();

     void updateProgress(qint64);

     void on_fileChooseButton_clicked();

//     void on_pushButton_clicked();

     void on_downloadFileButton_clicked();

     void on_uploadButton_clicked();

     void on_fileList_itemClicked(QTreeWidgetItem *item);

     void fillDropdown(QString msg);

     void updateUploadProgress();

     void fillFileList(QString msg);


     void on_deleteFileButton_clicked();

     void redirectToLogin(QString status);

     void on_insertUserButton_clicked();

     void on_removeUserButton_clicked();


private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    ServerInterface* si;

};

#endif // MAINWINDOW_H
