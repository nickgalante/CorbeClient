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

ServerInterface::ServerInterface(QObject *parent) : QObject(parent)
{

}

/*//not tested, probably will fail
bool ServerInterface::getFile(QString userid, QString filename) {

    QNetworkAccessManager manager;
    QNetworkRequest req(QUrl("http://localhost:8080/returnFile"));
    QNetworkReply* reply = manager.get(req);

    QFile file("hello.txt");
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();

    return true;
}*/

void ServerInterface::handleLogin(QString email, QString password){
    //ui->label->setText(ui->email->toPlainText());

    QNetworkAccessManager *manager = new QNetworkAccessManager();

     //QNetworkReply *reply;

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


/*int ServerInterface::postFile(QString userid, QString filename, QString filelocation) {

    // load file into memory from here

    QNetworkAccessManager manager;
    QNetworkRequest req(url);
    req.setRawHeader( "User-Agent" , "Meeting C++ RSS Reader" );
    QNetworkReply* reply = manager.post(req);
}*/



