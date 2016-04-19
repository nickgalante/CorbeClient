#ifndef UPLOADWORKER_H
#define UPLOADWORKER_H

#include <QFile>
#include <QNetworkAccessManager>
#include <QString>
#include <QUrl>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QThread>

class UploadWorker : public QObject{
    Q_OBJECT
//class UploadWorker : public QThread{


public:
    UploadWorker(QString, QString fileNameAndDirectory, QSslConfiguration *sslConfig);
    void run();

public slots:
    void erroron_filesend(QNetworkReply*);
    void fileUploadProgress(qint64 bytesSent, qint64 bytesTotal);

signals:
    void invalidTokenSignal(QString msg);

private:
    QString token;
    QString fileNameAndDirectory;
    long bytesRead;
//    QNetworkAccessManager* manager;
    QNetworkRequest* request;
    QFile* fileToSend;
    QSslConfiguration *sslConfig;



};

#endif // UPLOADWORKER_H
