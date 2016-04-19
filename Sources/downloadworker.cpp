#include "downloadworker.h"

#include <iostream>
#include <fstream>
#include <QFile>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QDebug>
#include <QSslConfiguration>
#include <QString>
#include <QProgressBar>



DownloadWorker::DownloadWorker(QString token, QString fileName, QString downloadToDirectory,
                               QNetworkReply* reply, QSslConfiguration *sslConfig){
    this->token = token;
    this->filename = fileName;
    this->downloadToDirectory = downloadToDirectory;
    this->reply = reply;
    this->reply->setSslConfiguration(*sslConfig);

    this->fileToWriteTo = new QFile(downloadToDirectory + fileName);
    this->totalWrote = 0;
    this->fileToWriteTo->open(QIODevice::WriteOnly);

    //file not good, throw exception while we're still ahead of the game.
    if(!this->fileToWriteTo->isWritable()){
        throw ("File was unable to be opened for writing: " + downloadToDirectory);
    }
}


void DownloadWorker::onHttpReadyRead(){

    QByteArray response = this->reply->readAll();

    QVariant statusCode = this->reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "status code: " << statusCode.toInt();
    if(statusCode != 200){
        qDebug() << "Invalid Token";
        emit invalidTokenSignal("Invalid");
    }
    else if (this->fileToWriteTo->isWritable()){
        this->totalWrote += this->fileToWriteTo->write(response);
        this->fileToWriteTo->seek(this->totalWrote);
        this->fileToWriteTo->flush();
    }
}


void DownloadWorker::downloadFinished(QNetworkReply *rep)
{
    qDebug() << "download finished";
    this->fileToWriteTo->close();
}
