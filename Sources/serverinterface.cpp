#include "serverinterface.h"
#include "mainwindow.h"


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

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req(QUrl("http://localhost:8080/returnFile"));
    rep = manager->get(req);

    file = new QFile("/Users/jgalante1/downloads/largefile.txt");
    file->open(QIODevice::WriteOnly);

    connect(rep, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

    connect(rep, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDownloadProgress(qint64, qint64)));

    return true;
}


void ServerInterface::downloadFinished(QNetworkReply *rep)
{
    qDebug() << "finished";
    file->close();
}


void ServerInterface::httpReadyRead()
{
    QByteArray data;
    if (file)
        data = rep->readAll();
    qDebug() << "writing";
        //file->write(data);
}

void ServerInterface::updateDownloadProgress(qint64 read, qint64 total)
{
    qDebug() << read << total;
    emit progressSignal(read, total);
}




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

        /*QByteArray boundary;

           QByteArray datas(QString("--" + boundary + "\r\n").toAscii());
           datas += "Content-Disposition: form-data; name=\"file\"; filename=\""+file.fileName()+"\"\r\n";

           datas += "Content-Type: image/jpeg\r\n\r\n"; //file type is here
           datas += line; //and our file is giving to form object
           datas += "\r\n";
           datas += QString("--" + boundary + "\r\n\r\n").toAscii();
           datas += "Content-Disposition: form-data; name=\"upload\"\r\n\r\n";
           datas += "Uploader\r\n";
           datas += QString("--" + boundary + "--\r\n").toAscii();
           */

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



