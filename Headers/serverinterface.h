#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QProgressBar>
#include <QFile>

class ServerInterface : public QObject
{
    Q_OBJECT
public:
    explicit ServerInterface(QObject *parent = 0);

    bool getFile();
    //void sendFile(QString userid, QString name, QString file_location);
    void sendFile(QString name);
    void handleLogin(QString email, QString password);
    void startDownload();


public slots:
   void replyFinished(QNetworkReply*);
   void downloadFinished(QNetworkReply*);
   void updateDownloadProgress(qint64 read, qint64 total);
   void httpReadyRead();

signals:
   void loginSignal(QString msg);
   void progressSignal(qint64 read, qint64 total);

private:
   QNetworkReply *rep;
   QFile *file;

};

#endif // SERVERINTERFACE_H
