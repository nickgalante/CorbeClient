#include "serverinterface.h"


#include <fstream>
#include <QFile>

bool ServerInterface::getFile(string userid, string filename) {

    QNetworkAccessManager manager;
    QNetworkRequest req(QUrl("http://localhost:8080/returnFile"));
    QNetworkReply* reply = manager.get(req);

    QFile file("hello.txt");
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();

    return true;
}

/*int ServerInterface::postFile(string userid, string filename, string filelocation) {

	// load file into memory from here

	QNetworkAccessManager manager;
	QNetworkRequest req(url);
	req.setRawHeader( "User-Agent" , "Meeting C++ RSS Reader" );
	QNetworkReply* reply = manager.post(req);
}*/
