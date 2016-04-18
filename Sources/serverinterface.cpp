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
#include <QSslKey>
#include <QSslConfiguration>
#include <QStandardPaths>
#include <QString>
#include <QProgressBar>

ServerInterface::ServerInterface(QObject *parent) : QObject(parent)
{

}


bool ServerInterface::getFile() {

   //create the usual crap
   QNetworkAccessManager *manager = new QNetworkAccessManager();
   QNetworkRequest req(QUrl("https://localhost:8443/returnFile"));
   req.setSslConfiguration(*this->sslConfig);

   //Create a new thread,a new dw worker, connect them all, and fire off the damn thing
   QString token = "";
   QString fileName = "thisshit.txt";
   QString downloadToDirectory = "/Users/jgalante1/Downloads/";

   this->reply = manager->get(req);
   QThread *t = new QThread();
   qDebug() << "In ServerInterface::getFile() << about to call the Download Worker constructor";
   DownloadWorker* dw = new DownloadWorker(token, fileName, downloadToDirectory, this->reply, this->sslConfig);
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
   QNetworkRequest req(QUrl("https://localhost:8443/decrypt"));
   req.setSslConfiguration(*this->sslConfig);
   req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

   //Create a new thread,a new dw worker, connect them all, and fire off the damn thing
   this->token = token;
   this->fileName = fileName;
   QString downloadToDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/";
   qDebug() << "Writing " << fileName << " to " << downloadToDirectory;
    qDebug() << this->token << this->fileName;
   QByteArray postData;
   postData.append("token=" + this->token + "&");
   postData.append("fileName=" + this->fileName);

   this->reply = manager->post(req, postData);
   QThread *t = new QThread();
   qDebug() << "In ServerInterface::getFile() << about to call the Download Worker constructor";
   DownloadWorker* dw = new DownloadWorker(this->token, this->fileName, downloadToDirectory, this->reply, this->sslConfig);
   connect(this->reply, SIGNAL(readyRead()), dw, SLOT(onHttpReadyRead()));

   manager->moveToThread(t);
   reply->moveToThread(t);
   dw->moveToThread(t);
   t->start();

   connect(manager, SIGNAL(finished(QNetworkReply*)), dw, SLOT(downloadFinished(QNetworkReply*)));
   //connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadStatus(QNetworkReply*)));
   connect(dw, SIGNAL(invalidTokenSignal(QString)), this, SLOT(downloadStatus(QString)));
   connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDownloadProgress(qint64, qint64)));

   return true;
}


void ServerInterface::updateDownloadProgress(qint64 read, qint64 total)
{
    qDebug() << read << total;
    emit progressSignal(read, total);
}

void ServerInterface::downloadStatus(QString msg){
    qDebug() <<"calling downloadStatus";
    qDebug() << "response" << msg;
    emit downloadStatusSignal(msg);

}

/**
 * @brief ServerInterface::handleLogin Establishes ssl and handles login
 * @param email
 * @param password
 */
void ServerInterface::handleLogin(QString email, QString password){

    this->establishSslConfig();

    QNetworkAccessManager *manager = new QNetworkAccessManager();

        QUrl url("https://localhost:8443/signin");
        QNetworkRequest request(url);
        request.setSslConfiguration(*this->sslConfig);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QByteArray postData;
        postData.append("email=" + email);
        postData.append("&password=" + password);

        manager->post(request, postData);

        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
        this->userEmail = email;


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

void ServerInterface::signout(){
    this->establishSslConfig();

    QNetworkAccessManager *manager = new QNetworkAccessManager();

        QUrl url("https://localhost:8443/signout");
        QNetworkRequest request(url);
        request.setSslConfiguration(*this->sslConfig);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QByteArray postData;
        postData.append("token=" + this->token);
        qDebug() <<"token: " << this->token;

        manager->post(request, postData);

        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(signoutFinished(QNetworkReply*)));
}

void ServerInterface::signoutFinished(QNetworkReply *reply)
{
    QString data = reply->readAll();
        if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::replyFinished has received null or empty data.";
        } else{

            emit signoutSignal(data);
        }
}


//void ServerInterface::sendFile(QString userid, QString filename, QString filelocation)
void ServerInterface::sendFile(QString filename){

    //connect(this->reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(updateUploadProgress(qint64, qint64)));


    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QUrl url("https://localhost:8443/encrypt");
    QNetworkRequest request(url);
    request.setSslConfiguration(*this->sslConfig);
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
QString ServerInterface::getUserEmail(){
    return this->userEmail;
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

    //connect(this->reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(updateUploadProgress(qint64, qint64)));

    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) fileNameAndDirectory: " << fileNameAndDirectory;
    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) token: " << this->token;
    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) making the upload worker";
    UploadWorker* uploadWorker = new UploadWorker(this->token, fileNameAndDirectory, this->sslConfig);
    QThread* t = new QThread();
    uploadWorker->moveToThread(t);
    uploadWorker->run();
    t->start();

    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) already started";
}

void ServerInterface::updateUploadProgress(qint64 read, qint64 total)
{
    qDebug()<< "upload" << read << total;
    emit uploadProgressSignal(read, total);
}

//disabled for now
bool ServerInterface::isServerContactable(){
//    QTcpSocket socketToServer;
//    QHostAddress serverAddress("127.0.0.1");
//    socketToServer.connectToHost(serverAddress, 8080);
//    return socketToServer.waitForConnected(2000); //waits 2 seconds...
    return true;
}

void ServerInterface::getSubordiantes(){
    QNetworkAccessManager *manager = new QNetworkAccessManager();

        QUrl url("https://localhost:8443/getDirectSubordinates");
        QNetworkRequest request(url);
        request.setSslConfiguration(*this->sslConfig);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QByteArray postData;
        postData.append("token=" + this->token);
        postData.append("&userEmail=" + this->userEmail);

        manager->post(request, postData);

        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getSubordiantesFinished(QNetworkReply*)));
}

void ServerInterface::getSubordiantesFinished(QNetworkReply *reply)
{

    QString data = reply->readAll();
    if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::replyFinished has received null or empty data.";
        } else{

            emit getSubordiantesSignal(data);
        }
}

void ServerInterface::deleteFile(QString fileName, QString token){
    //create the usual crap
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req(QUrl("https://localhost:8443/deleteFile"));
    req.setSslConfiguration(*this->sslConfig);

    this->token = token;
    this->fileName = fileName;

    QByteArray postData;
    postData.append("token=" + this->token + "&");
    postData.append("fileName=" + this->fileName);

    this->reply = manager->post(req, postData);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(deleteFinished(QNetworkReply*)));

}

void ServerInterface::deleteFinished(QNetworkReply *rep)
{
    qDebug() << "File Successfully Deleted";
}

void ServerInterface::getUserFileList(QString user){
    qDebug() <<"User File List for: " << user;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(userFileListResponse(QNetworkReply*)));
    QNetworkRequest req(QUrl("https://localhost:8443/UserFileList"));
    req.setSslConfiguration(*this->sslConfig);

    QByteArray postData;
    postData.append("email=" + user);

    this->reply = manager->post(req, postData);


}

void ServerInterface::userFileListResponse(QNetworkReply *reply){
    QString data = reply->readAll();
    if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::UserFileListResponse has received null or empty data.";
        } else{

            emit userFileListSignal(data);
        }
}

void ServerInterface::insertNewUser(QString email, QString firstName, QString lastName, QString department, QString superior, QString password){
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req(QUrl("https://localhost:8443/insertNewUser"));
    req.setSslConfiguration(*this->sslConfig);

    QByteArray postData;
    postData.append("token=" + this->token + "&");
    postData.append("newUserEmail=" + email + "&");
    postData.append("newUserFirstName=" + firstName + "&");
    postData.append("newUserLastName=" + lastName + "&");
    postData.append("newUserDepartment=" + department + "&");
    postData.append("newUserSuperiorEmail=" + superior + "&");
    postData.append("newUserPassword=" + password);

    this->reply = manager->post(req, postData);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(insertFinished(QNetworkReply*)));
}

void ServerInterface::insertFinished(QNetworkReply* reply){
    qDebug() << "User inserted " << reply->readAll();
}

void ServerInterface::removeUser(QString email){
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req(QUrl("https://localhost:8443/removeUser"));
    req.setSslConfiguration(*this->sslConfig);

    QByteArray postData;
    postData.append("token=" + this->token + "&");
    postData.append("emailUserToRemove=" + email);

    this->reply = manager->post(req, postData);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(removeFinished(QNetworkReply*)));
}

void ServerInterface::removeFinished(QNetworkReply* reply){
    qDebug() << "User removed" << reply->readAll();
}

void ServerInterface::establishSslConfig(){

    QFile certLocation(":/ssl/Resources/CORBE_Cert_Distribute.der");
    certLocation.open(QIODevice::ReadOnly);
    QSslCertificate cert(&certLocation, QSsl::Der);

    this->sslConfig = new QSslConfiguration();
    QSslKey key(cert.toDer(), QSsl::Rsa, QSsl::Der, QSsl::PrivateKey);
    sslConfig->setCaCertificates(QList<QSslCertificate>() << cert);
    sslConfig->setPrivateKey(key);
    sslConfig->setPeerVerifyMode(QSslSocket::QueryPeer);

}
