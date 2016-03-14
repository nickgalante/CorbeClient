#include "serverinterface.h"

#include <iostream>
#include <fstream>
#include <QFile>

//not tested, probably will fail
bool ServerInterface::getFile(QString userid, QString filename) {

    QNetworkAccessManager manager;
    QNetworkRequest req(QUrl("http://localhost:8080/returnFile"));
    QNetworkReply* reply = manager.get(req);

    QFile file("hello.txt");
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();

    return true;
}

void ServerInterface::handleLogin(QString email, QString password){

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QUrl url("http://localhost:8080/signin");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray postData;
    postData.append("email=" + email);
    postData.append("&md5Password=" +  password);

    manager->post(request, postData);

}


/*int ServerInterface::postFile(QString userid, QString filename, QString filelocation) {

	// load file into memory from here

	QNetworkAccessManager manager;
	QNetworkRequest req(url);
	req.setRawHeader( "User-Agent" , "Meeting C++ RSS Reader" );
	QNetworkReply* reply = manager.post(req);
}*/
