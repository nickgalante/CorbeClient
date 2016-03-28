#include "serverinterface.h"
#include "mainwindow.h"
#include "downloadworker.h"


#include <iostream>
#include <fstream>
#include <QFile>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QDebug>
#include <QString>
#include <QProgressBar>

ServerInterface::ServerInterface(QObject *parent) : QObject(parent)
{

}


bool ServerInterface::getFile() {

    QString token = "blahToken";
    QString email = "BlahEmail";
    QString fileName = "BlachFileName";

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req(QUrl("http://localhost:8080/returnFile"));


    DownloadWorker* dwptr = new DownloadWorker(token, email, fileName, manager, req);


    connect(dwptr->rep, SIGNAL(readyRead()), dwptr, SLOT(httpReadyRead()));
    connect(dwptr->manager, SIGNAL(finished(QNetworkReply*)), dwptr, SLOT(downloadFinished(QNetworkReply*)));
    connect(dwptr->rep, SIGNAL(downloadProgress(qint64, qint64)), dwptr, SLOT(updateDownloadProgress(qint64, qint64)));

    qDebug()<<"From main thread: "<<QThread::currentThreadId();
    dwptr->start();
    return true;
}


/*void ServerInterface::downloadFinished(QNetworkReply *rep)
{
    qDebug() << "finished";
    file->close();
}*/


/*void ServerInterface::httpReadyRead()
{
    QByteArray data;
    if (file)
        data = rep->readAll();
    qDebug() << "writing";
        file->write(data);
}*/

/*void ServerInterface::updateDownloadProgress(qint64 read, qint64 total)
{
    qDebug() << read << total;
    emit progressSignal(read, total);
}
*/



void ServerInterface::handleLogin(QString email, QString password){

    QNetworkAccessManager *manager = new QNetworkAccessManager();

        QUrl url("http://localhost:8080/signin");
        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QByteArray postData;
        postData.append("email=" + email);
        postData.append("&password=" + password);

        manager->post(request, postData);

        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));


}
void ServerInterface::replyFinished(QNetworkReply *reply)
{
    QString data = reply->readAll();
    emit loginSignal(data);
}


//void ServerInterface::sendFile(QString userid, QString filename, QString filelocation)
void ServerInterface::sendFile(QString filename){


        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)) {
             qDebug() << "file open failure";
        }

        QByteArray line;
           while (!file.atEnd())
           {
               line.append(file.readLine());
           }

        file.close();

        QNetworkAccessManager *manager = new QNetworkAccessManager();

            QUrl url("http://localhost:8080/encrypt");
            QNetworkRequest request(url);

            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

           //connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(erroron_filesend(QNetworkReply*)));
            qDebug() << "Sending file";
           manager->post(request,line); //send all data
           qDebug() << "file Sent";
        //QTextStream in(&file);
        //ui->textBrowser->setText(in.readAll());


}



