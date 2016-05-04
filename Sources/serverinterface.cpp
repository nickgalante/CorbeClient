/**
@file
*/
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

/*!
 * calls the serverController decrypt function and downloads a file, multithreaded for performance
 * @brief ServerInterface::getFile
 * @return
 */
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

/**
 * Calls the ServerController decrypt function and downloads a specified User file, multithreaded for performanceoc
 * @brief ServerInterface::getFile
 * @param fileName Name of the file to download
 * @param token User's unique session token
 * @param lookupEmail Email of the file's owner
 * @return
 */
bool ServerInterface::getFile(QString fileName, QString token, QString lookupEmail) {
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
   postData.append("&lookupEmail=" + lookupEmail);

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
   connect(dw, SIGNAL(invalidTokenSignal(QString)), this, SLOT(downloadStatus(QString)));
   connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDownloadProgress(qint64, qint64)));

   return true;
}

/**
 * Update the download progress Bar
 * @brief ServerInterface::updateDownloadProgress
 * @param read
 * @param total
 */
void ServerInterface::updateDownloadProgress(qint64 read, qint64 total)
{
    qDebug() << read << total;
    emit progressSignal(read, total);
}

/**
 * Returns the status of the download whether its a success or failure
 * @brief ServerInterface::downloadStatus
 * @param msg
 */
void ServerInterface::downloadStatus(QString msg){
    qDebug() <<"calling downloadStatus";
    qDebug() << "response" << msg;
    emit invalidStatusCodeSignal(msg);

}

/**
 * @brief ServerInterface::handleLogin Establishes ssl and handles login
 * @param email User's entered email
 * @param password User's entered password
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

/**
 * Login Request Finished, Signals the mainWindow whether the login was a success or failure
 * @brief ServerInterface::replyFinished
 * @param reply
 */
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

/**
 * Calls the ServerCOntroller signout function invalidates a session token
 * @brief ServerInterface::signout
 */
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

/**
 * Signout request finished, retunrs success or failure
 * @brief ServerInterface::signoutFinished
 * @param reply
 */
void ServerInterface::signoutFinished(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "status code: " << statusCode.toInt();
    QString data = reply->readAll();
        if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::replyFinished has received null or empty data.";
        } else if(statusCode == 200){

            emit signoutSignal("Success");
        }
        else{
            emit signoutSignal("Invalid Attempt");
        }
}


//void ServerInterface::sendFile(QString userid, QString filename, QString filelocation)
/**
 * Upload a specified user file, calls the encrypt function
 * @brief ServerInterface::sendFile
 * @param filename Name of the file
 */
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

/**
 * @brief ServerInterface::getToken
 * @return The Current user's token
 */
QString ServerInterface::getToken(){
    return this->token;
}

/**
 * @brief ServerInterface::getUserEmail
 * @return  The current user's email
 */
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
/**
 * Upload a specified file to the server, calls the encrypt function, Multithreaded for performance
 * @brief ServerInterface::uploadFile
 * @param fileNameAndDirectory Name of the file to upload
 */
void ServerInterface::uploadFile(QString fileNameAndDirectory){

    //connect(this->reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(updateUploadProgress(qint64, qint64)));
    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) fileNameAndDirectory: " << fileNameAndDirectory;
    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) token: " << this->token;
    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) making the upload worker";
    UploadWorker* uploadWorker = new UploadWorker(this->token, fileNameAndDirectory, this->sslConfig);

    connect(uploadWorker, SIGNAL(invalidTokenSignal(QString)), this, SLOT(uploadStatus(QString)));
    connect(uploadWorker, SIGNAL(uploadSuccessSignal()), this, SLOT(updateUploadProgress()));

    QThread* t = new QThread();
    uploadWorker->moveToThread(t);
    uploadWorker->run();
    t->start();

    qDebug() << "ServerInterface::uploadFile(QString fileNameAndDirectory) already started";
}

void ServerInterface::updateUploadProgress()
{
    qDebug() << "emitting upload success from serverInterface";
    emit uploadSuccess();
}

void ServerInterface::uploadStatus(QString msg){
    qDebug() <<"calling uploadStatus";
    qDebug() << "response" << msg;
    emit invalidStatusCodeSignal(msg);
}

//disabled for now
bool ServerInterface::isServerContactable(){
//    QTcpSocket socketToServer;
//    QHostAddress serverAddress("127.0.0.1");
//    socketToServer.connectToHost(serverAddress, 8080);
//    return socketToServer.waitForConnected(2000); //waits 2 seconds...
    return true;
}

/**
 * Returns the Current user's subordinates
 * @brief ServerInterface::getSubordiantes
 */
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

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Get Subordiantes status code: " << statusCode.toInt();
    QString data = reply->readAll();
        if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::getSubordiantesFinished has received null or empty data.";
        } else if(statusCode == 200){

            emit getSubordiantesSignal(data);

        }
        else{
            emit invalidStatusCodeSignal("Invalid Token");
        }

}

/**
 * Deletes a specified file.Calls the serverController Delete function
 * @brief ServerInterface::deleteFile
 * @param fileName Name of the file to delete
 * @param token Current user's session token
 */
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
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Insert User status code: " << statusCode.toInt();
    QString data = reply->readAll();
        if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::deleteFinished has received null or empty data.";
        } else if(statusCode == 200){
            qDebug() << "File Successfully Deleted";

        }
        else{
            emit invalidStatusCodeSignal("Invalid Token");
        }


}

/**
 * Returns the File List of a specified user
 * @brief ServerInterface::getUserFileList
 * @param user Target user
 */
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

/**
 * Insert a new user into the employee tree. Calls the serverController insertNewUser function
 * @brief ServerInterface::insertNewUser
 * @param email New email
 * @param firstName New first Name
 * @param lastName New Last name
 * @param department User department
 * @param superior user's superior email
 * @param password User's given password
 */
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

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Insert User status code: " << statusCode.toInt();
    QString data = reply->readAll();
        if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::getSubordiantesFinished has received null or empty data.";
        } else if(statusCode == 200){
            qDebug() << "User inserted " << reply->readAll();
            getSubordiantes();

        }
        else{
            emit invalidStatusCodeSignal("Invalid Token");
        }

}

/**
 * Remove a User from the employee tree. Calls the serverController remove function
 * @brief ServerInterface::removeUser
 * @param email email of user to remove
 */
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

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Get Subordiantes status code: " << statusCode.toInt();
    QString data = reply->readAll();
        if(data.isNull() || data.isEmpty()){
            throw "ServerInterface::getSubordiantesFinished has received null or empty data.";
        } else if(statusCode == 200){

            qDebug() << "User removed" << reply->readAll();
            getSubordiantes();

        }
        else{
            emit invalidStatusCodeSignal("Invalid Token");
        }

}

/**
 * Configure the ssl certificate
 * @brief ServerInterface::establishSslConfig
 */
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
