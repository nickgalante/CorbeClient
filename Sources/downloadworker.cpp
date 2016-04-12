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



DownloadWorker::DownloadWorker(QString token, QString fileName, QString downloadToDirectory, QNetworkReply* reply){
    this->token = token;
    this->filename = fileName;
    this->downloadToDirectory = downloadToDirectory;
    this->reply = reply;

    this->fileToWriteTo = new QFile(downloadToDirectory + fileName);
    this->totalWrote = 0;
    this->fileToWriteTo->open(QIODevice::WriteOnly);

    //file not good, throw exception while we're still ahead of the game.
    if(!this->fileToWriteTo->isWritable()){
        throw ("File was unable to be opened for writing: " + downloadToDirectory);
    }
}


void DownloadWorker::onHttpReadyRead(){

    QByteArray data;
    QString response = data;
    response = this->reply->readAll();
    if(response.contains("invalid token", Qt::CaseInsensitive)){
        qDebug() << "Invalid Token";
        emit invalidTokenSignal(response);

    }
    else if (this->fileToWriteTo->isWritable()){

        //qDebug() << "data" << data;
        this->totalWrote += this->fileToWriteTo->write(data);
        this->fileToWriteTo->seek(this->totalWrote);
        this->fileToWriteTo->flush();
    }
}


void DownloadWorker::downloadFinished(QNetworkReply *rep)
{
    qDebug() << "download finished";
    this->fileToWriteTo->close();
}
