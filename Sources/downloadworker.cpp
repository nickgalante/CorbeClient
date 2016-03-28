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

//requesting your own file
DownloadWorker::DownloadWorker(QString token, QString fileName, QNetworkReply *rep){
    this->token = token;
    this->fileName = fileName;
    this->totalWrote = 0;
}


//requesting someone elses.
DownloadWorker::DownloadWorker(QString token, QString email, QString fileName, QNetworkAccessManager *manager, QNetworkRequest req){
    this->token = token;
    this->email = email;
    this->fileName = fileName;
    this->manager = manager;
    this->req = req;
    this->rep = this->manager->get(this->req);
    this->totalWrote = 0;
}

void DownloadWorker::run(){
    qDebug()<<"From child thread RUN: "<<QThread::currentThreadId();

    this->file = new QFile("/Users/jgalante1/Downloads/LargeFile.txt"); //saves it here, if filewriting is enabled
    if(this->file->open(QIODevice::WriteOnly)){
        qDebug() << "File Has Been Created" << endl;

    }
    else{
        qDebug() << "Failed to Create File" << endl;
    }
}

void DownloadWorker::httpReadyRead()
{
    qDebug()<<"From child thread httpreadyread: "<<QThread::currentThreadId();
    QByteArray data;
    if (this->file){
        data = rep->readAll();
        this->totalWrote += this->file->write(data);
        this->file->seek(this->totalWrote);
        qDebug() << "Wrote this many bytes: " << this->totalWrote;
    }

}

void DownloadWorker::downloadFinished(QNetworkReply *rep)
{
    qDebug() << "finished";
    this->file->close();
}

void DownloadWorker::updateDownloadProgress(qint64 read, qint64 total)
{
    qDebug() << "updateDownloadProgress: " << read << total;
    emit progressSignal(read, total);
}



