#include "serverinterface.h"
#include "mainwindow.h"
#include "downloadworker.h"
#include "uploadworker.h"


#include <iostream>
#include <fstream>
#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QDebug>
#include <QStandardPaths>
#include <QString>
#include <QProgressBar>

ServerInterface::ServerInterface(QObject *parent) : QObject(parent)
{

}


bool ServerInterface::getFile() {

   //create the usual crap
   QNetworkAccessManager *manager = new QNetworkAccessManager();
   QNetworkRequest req(QUrl("http://localhost:8080/returnFile"));

   //Create a new thread,a new dw worker, connect them all, and fire off the damn thing
   QString token = "";
   QString fileName = "thisshit.txt";
   QString downloadToDirectory = "/Users/jgalante1/Downloads/";

   this->reply = manager->get(req);
   QThread *t = new QThread();
   qDebug() << "In ServerInterface::getFile() << about to call the Download Worker constructor";
   DownloadWorker* dw = new DownloadWorker(token, fileName, downloadToDirectory, this->reply);
   connect(this->reply, SIGNAL(readyRead()), dw, SLOT(onHttpReadyRead()));

   manager->moveToThread(t);
   reply->moveToThread(t);
   dw->moveToThread(t);
   t->start();

   //connect(manager, SIGNAL(finished(QNetworkReply*)), dw, SLOT(downloadFinished(QNetworkReply*)));
   connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDownloadProgress(qint64, qint64)));

   return true;
}

bool ServerInterface::getFile(QString fileName, QString token) {

   //create the usual crap
   QNetworkAccessManager *manager = new QNetworkAccessManager();
   QNetworkRequest req(QUrl("http://localhost:8080/decrypt"));

   //Create a new thread,a new dw worker, connect them all, and fire off the damn thing
   this->token = token;
   this->fileName = fileName;
   QString downloadToDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/";
   qDebug() << "Writing " << fileName << " to " << downloadToDirectory;

   QByteArray postData;
   postData.append("token=" + this->token + "&");
   postData.append("fileName=" + this->fileName);

   this->reply = manager->post(req, postData);
   QThread *t = new QThread();
   qDebug() << "In ServerInterface::getFile() << about to call the Download Worker constructor";
   DownloadWorker* dw = new DownloadWorker(this->token, this->fileName, downloadToDirectory, this->reply);
   connect(this->reply, SIGNAL(readyRead()), dw, SLOT(onHttpReadyRead()));

   manager->moveToThread(t);
   reply->moveToThread(t);
   dw->moveToThread(t);
   t->start();

   connect(manager, SIGNAL(finished(QNetworkReply*)), dw, SLOT(downloadFinished(QNetworkReply*)));
   connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDownloadProgress(qint64, qint64)));

   return true;
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
        if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::replyFinished has received null or empty data.";
        } else{
            //extracts out the token from a successful response.
            QString delimiter = "Token:";
            int indexOfToken = data.indexOf(delimiter);
            QString token = data.mid(delimiter.size() + indexOfToken);
            this->token = token;

            emit loginSignal(data);
        }
}



//void ServerInterface::sendFile(QString userid, QString filename, QString filelocation)
void ServerInterface::sendFile(QString filename){


    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QUrl url("http://localhost:8080/encrypt");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("token", this->token.toUtf8());
    request.setRawHeader("fileName", filename.toUtf8());


    QFile* fileToSend = new QFile(filename);
    if(!fileToSend->open(QIODevice::ReadOnly) || !fileToSend->isReadable()) {
        qDebug() << "file open failure";
        throw "UNABLE TO OPEN THE FILE.";
    }

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart clearTextFile;
    clearTextFile.setBodyDevice(fileToSend);
    fileToSend->setParent(multiPart);
    multiPart->append(clearTextFile);


    //connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(erroron_filesend(QNetworkReply*)));
    qDebug() << "Sending file";

    QByteArray postData;
    postData.append("email=ceo@test.com");
    postData.append("&password=password");

    QNetworkReply* reply = manager->post(request, postData);
    connect(reply, SIGNAL(finished()), this, SLOT(uploadReplyFinished()));
    multiPart->setParent(reply);


//    manager->post(request, &file);
    qDebug() << "About to close the file.";
    fileToSend->close();
    qDebug() << "Closed the file.";




    //QTextStream in(&file);
    //ui->textBrowser->setText(in.readAll());
}

QString ServerInterface::getToken(){
    return this->token;
}


void ServerInterface::uploadReplyFinished()
{
    qDebug() << "ServerInterface::uploadReplyFinished called";
//    QString data = reply->readAll();
//    if(data.isNull() || data.isEmpty()){
//        throw "ServerInterface::replyFinished has received null or empty data.";
//    } else{
//        qDebug() << "Upload reply finished? " << data;
//    }
}


//MIGHT NEED TO STORE THE UPLOADWORKER* AS A GLOBAL/EXTERNAL VARIABLE
void ServerInterface::uploadFile(QString fileNameAndDirectory){

    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) fileNameAndDirectory: " << fileNameAndDirectory;
    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) token: " << this->token;
    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) making the upload worker";
    UploadWorker* uploadWorker = new UploadWorker(this->token, fileNameAndDirectory);
    QThread* t = new QThread();
    uploadWorker->moveToThread(t);
    uploadWorker->run();
    t->start();

    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) already started";
}


bool ServerInterface::isServerContactable(){
    QTcpSocket socketToServer;
    QHostAddress serverAddress("127.0.0.1");
    socketToServer.connectToHost(serverAddress, 8080);
    return socketToServer.waitForConnected(2000); //waits 2 seconds...
}



