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
    bool getFile(QString fileName, QString token);
    //void sendFile(QString userid, QString name, QString file_location);
    void sendFile(QString name);
    void uploadFile(QString fileNameAndDirectory); //the one we use
    void handleLogin(QString email, QString password);
    void startDownload();
    QString getToken();
    QString getUserEmail();
    bool isServerContactable();
    void getSubordiantes();

public slots:
   void replyFinished(QNetworkReply*);
   void uploadReplyFinished(); //when the upload is complete do this
   //void downloadFinished(QNetworkReply*);
   void updateDownloadProgress(qint64 read, qint64 total);
   void getSubordiantesFinished(QNetworkReply*);
   //void httpReadyRead();

signals:
   void loginSignal(QString msg);
   void progressSignal(qint64 read, qint64 total);
   void getSubordiantesSignal(QString msg);

private:
   QString token;
   QString fileName;
   QNetworkReply *reply;
   QFile *file;
   QString userEmail;
};

#endif // SERVERINTERFACE_H

